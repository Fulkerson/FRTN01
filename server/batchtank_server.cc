#include <algorithm>
#include <iostream>
#include <string>
#include <time.h>
#include <cerrno>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

//#define DEBUG

#ifndef D
#ifdef DEBUG
#define D
#else
#define D if(0)
#endif
#endif

#include "batchtank_server.h"
#include "../common/message_utils.h"

/* Used to silence compiler when unused vars. */
#define UNUSED(expr) if(0) { (void)(expr); }


using boost::asio::ip::tcp;
using namespace batchtank;

/* Convenience function for adding two timespecs */
static timespec
add_timespec(timespec t1, timespec t2)
{
    timespec t;
    t.tv_sec  = t1.tv_sec + t2.tv_sec;
    t.tv_sec += (t1.tv_nsec + t2.tv_nsec) / 999999999;
    t.tv_nsec = (t1.tv_nsec + t2.tv_nsec) % 999999999;
    return t;
}


double
IORegistry::getSensor(messages::SensorType type)
{
    std::cout << "GET ";
    switch(type) {
        case messages::HEATERSENSOR:
            std::cout << "HEATERSENSOR: ";
            break;
        case messages::COOLERSENSOR:
            std::cout << "COOLERSENSOR: ";
            break;
        case messages::INLETSENSOR:
            std::cout << "INLETSENSOR: ";
            break;
        case messages::OUTLETSENSOR:
            std::cout << "OUTLETSENSOR: ";
            break;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            break;
    }
    std::cout << 2 << std::endl;
    return 2;
}

void
IORegistry::setSensor(messages::SensorType type, double value)
{
    std::cout << "SET ";
    switch(type) {
        case messages::HEATER:
            std::cout << "HEATER: ";
            break;
        case messages::COOLER:
            std::cout << "COOLER: ";
            break;
        case messages::INLETPUMP:
            std::cout << "INLETPUMP: ";
            break;
        case messages::OUTLETPUMP:
            std::cout << "OUTLETPUMP: ";
            break;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            break;
    }
    std::cout << value << std::endl;
}



Sampler::Sampler(std::vector<messages::SensorType>& sensors, IORegistry& ioreg,
        tcp::socket& sock):
    sensors(sensors), ioreg(ioreg), m_Socket(sock) {}


void 
Sampler::operator()()
{
    /* Construct output stream */
    MessageOutput<messages::BaseMessage, tcp::socket> out(m_Socket);

    /* Construct message */
    msg.Clear();

    {
        /* Sample and create message */
        boost::lock_guard<boost::mutex> lock(ioreg.mutex);
        
        std::for_each (sensors.begin(), sensors.end(),
               [this](messages::SensorType type) {
            messages::Sample* sample = msg.add_sample();
            sample->set_value(ioreg.getSensor(type));
            sample->set_type((messages::Sensor) type);
        });
    }

    out << msg;
}


PeriodicTask::PeriodicTask(int32_t period, std::function<void()> task) :
    period(period), task(task){}

PeriodicTask::~PeriodicTask()
{
    this->stop();
}

void
PeriodicTask::start()
{
    D std::cout << "Starting periodic task" << std::endl;
    run = true;
    m_Thread = boost::thread(&PeriodicTask::execute, this);
}

void
PeriodicTask::stop()
{
    D std::cout << "Stopping periodic task" << std::endl;
    run = false;
    m_Thread.join();
}

void
PeriodicTask::execute()
{
    timespec t; 
    timespec p;

    /* Period is in millis, convert to sec and nanos */
    p.tv_sec  = period / 1000;
    p.tv_nsec = (period % 1000) * 1000000L;

    clock_gettime(CLOCK_MONOTONIC, &t);

    while (run) {
        /* Execute given task */
        task();

        /* Calc and sleep until next period. */
        t = add_timespec(t, p);

        while (true) {
            if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, nullptr)) {
                switch (errno) {
                    case EINTR:
                        std::cerr << "EINTR" << std::endl;
                        /* Try again if interupted */
                        continue;
                    case EFAULT:
                        std::cerr << "EFAULT" << std::endl;
                        break;
                    case EINVAL:
                        std::cerr << "EINVAL" << std::endl;
                        break;
                }
                run = false;
                std::cerr << "ERROR: Leaving periodic task" << std::endl;
                break;
            } else {
                /* All good */
                break;
            }
        }
    }
}


ConnectionThread::ConnectionThread(std::shared_ptr<tcp::socket> m_Socket,
        IORegistry& ioreg):  m_Socket(m_Socket), ioreg(ioreg) {}


ConnectionThread::~ConnectionThread()
{
    m_Socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    m_Socket->close();
}


void
ConnectionThread::start()
{
    m_Thread = boost::thread(&ConnectionThread::run, this);
    m_Thread.detach();
}


void
ConnectionThread::run()
{
    std::string connected_to(m_Socket->remote_endpoint().address().to_string());

    try {
        MessageInput<messages::BaseMessage, tcp::socket> in(*m_Socket);
       
        std::unique_ptr<PeriodicTask> sampler;

        messages::BaseMessage msg;

        while (true) {
            msg.Clear();

            /* Parse message */
            in >> msg;

            D std::cout << "DEBUG: " << msg.DebugString() << std::endl;

            auto start = msg.signal().begin();
            auto end = msg.signal().end();
            
            {
                boost::lock_guard<boost::mutex> lock(ioreg.mutex);
                std::for_each (start, end, [this](messages::ControlSignal c) {
                    ioreg.setSensor(c.type(), c.value());
                });
            }

            /* Handle register for new sensors message */
            if (msg.has_register_()) {
                D std::cout << "Got registration" << std::endl;
                const messages::Register& r =  msg.register_();

                auto repeated  = r.type();
                std::vector<messages::SensorType> sensors(repeated.begin(),
                        repeated.end());

                /* Replace periodic timer */
                sampler.reset(new PeriodicTask(r.periodtime(),
                            Sampler(sensors, ioreg, *m_Socket)));
                sampler->start();
            }

            /* Shut down gracefully */
            if (msg.has_endconnection() && msg.endconnection()) {
                std::cerr << "Client: " << connected_to
                    << " asks to gracefully end connection" << std::endl;
                break;
            }

        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Closing connection to: " << connected_to << std::endl;
    }
    delete this;
}


int
main()
{
    try {
        /* IO monitor for batchtank process */
        IORegistry ioreg;

        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 54000);
        tcp::acceptor acceptor(io_service, endpoint);

        while(true) {
            std::shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
            acceptor.accept(*socket);

            (new ConnectionThread(socket, ioreg))->start();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
