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

IORegistry::IORegistry()
{
    /* Set values for signals */
}

int32_t
IORegistry::getOutput(messages::OutputType type)
{
    switch(type) {
        case messages::HEATER:
            return heater;
        case messages::COOLER:
            return cooler;
        case messages::IN_PUMP:
            return in_pump;
        case messages::OUT_PUMP:
            return out_pump;
        case messages::MIXER:
            return mixer;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            return 0;
    } 
}

int32_t
IORegistry::getReference(messages::OutputType type)
{
    switch(type) {
        case messages::HEATER:
            return heater_ref;
        case messages::COOLER:
            return cooler_ref;
        case messages::IN_PUMP:
            return in_pump_ref;
        case messages::OUT_PUMP:
            return out_pump_ref;
        case messages::MIXER:
            return mixer_ref;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            return 0;
    } 
}

int32_t
IORegistry::getSensor(messages::SensorType type)
{
    std::cout << "GET ";
    switch(type) {
        case messages::TEMP:
            std::cout << "TEMP: ";
            break;
        case messages::LEVEL:
            std::cout << "LEVEL: ";
            break;
        case messages::IN_PUMP_RATE:
            std::cout << "IN_PUMP_RATE: ";
            break;
        case messages::OUT_PUMP_RATE:
            std::cout << "OUT_PUMP_RATE: ";
            break;
        case messages::HEATER_RATE:
            std::cout << "HEATER_RATE: ";
            break;
        case messages::MIXER_RATE:
            std::cout << "MIXER_RATE: ";
            break;
        case messages::COOLER_RATE:
            std::cout << "COOLER_RATE: ";
            break;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            break;
    }
    std::cout << 2 << std::endl;
    return 2;
}

void
IORegistry::setOutput(messages::OutputType type, int32_t value, int32_t ref)
{
    std::cout << "SET ";
    switch(type) {
        case messages::HEATER:
            std::cout << "HEATER: ";
            heater = value;
            heater_ref = ref;
            break;
        case messages::COOLER:
            std::cout << "COOLER: ";
            cooler = value;
            cooler_ref = ref;
            break;
        case messages::IN_PUMP:
            std::cout << "IN_PUMP: ";
            in_pump = value;
            in_pump_ref = ref;
            break;
        case messages::OUT_PUMP:
            std::cout << "OUT_PUMP: ";
            out_pump = value;
            out_pump_ref = ref;
            break;
        case messages::MIXER:
            std::cout << "MIXER: ";
            mixer = value;
            mixer_ref = ref;
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
        MessageOutput<messages::BaseMessage, tcp::socket> out(*m_Socket);
       
        std::unique_ptr<PeriodicTask> sampler;

        messages::BaseMessage msg;
        messages::BaseMessage send;

        while (true) {
            msg.Clear();
            send.Clear();

            /* Parse message */
            in >> msg;

            D std::cout << "DEBUG: " << msg.DebugString() << std::endl;
            
            /* Handle signals */
            {
                auto start = msg.signal().begin();
                auto end = msg.signal().end();
                boost::lock_guard<boost::mutex> lock(ioreg.mutex);
                std::for_each (start, end, [this](messages::ControlSignal c) {
                    ioreg.setOutput(c.type(), c.value(), c.ref());
                });
            }

            /* Handle getSensor */
            {
                auto start = msg.getsensor().begin();
                auto end = msg.getsensor().end();
                boost::lock_guard<boost::mutex> lock(ioreg.mutex);
                std::for_each (start, end, [this, &send](messages::SensorType type) {
                    messages::Sample* s = send.add_sample();
                    s->set_value(ioreg.getSensor(type));
                    s->set_type((messages::Sensor) type);
                }); 
            }

            /* Handle getOutput */
            {
                auto start = msg.getoutput().begin();
                auto end = msg.getoutput().end();
                boost::lock_guard<boost::mutex> lock(ioreg.mutex);
                std::for_each (start, end, [this, &send](messages::OutputType type) {
                    messages::ControlSignal* s = send.add_signal();
                    s->set_value(ioreg.getOutput(type));
                    s->set_ref(ioreg.getReference(type));
                    s->set_type((messages::Output) type);
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

            /* Send if enything to send */
            if (send.ByteSize()) {
                out << send;
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
