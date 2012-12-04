#include <algorithm>
#include <iostream>
#include <string>
#include <time.h>
#include <cerrno>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <csignal>

//#define DEBUG

#ifndef D
#ifdef DEBUG
#define D
#else
#define D if(0)
#endif
#endif

#ifdef DUMMY_PROCESS
#include "dummy_cook.h"
#else
#include "../cook/cook.h"
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

IORegistry::IORegistry(std::string tty)
{
    if (init(tty.c_str()) == -1) {
        throw init_error;
    }
    std::cout << "Batchtank process initialized." << std::endl;
    /* Set values for signals */
    heater = get(HEATER_RATE);
    heater_ref = 0;
    cooler = get(COOLER_RATE);
    cooler_ref = 0;
    in_pump = get(IN_PUMP_RATE);
    in_pump_ref = 0;
    out_pump = get(OUT_PUMP_RATE);
    out_pump_ref = 0;
    mixer = get(MIXER_RATE);
    mixer_ref = 0;
}

IORegistry::~IORegistry()
{
    std::cout << "Destroying batchtank process." << std::endl;
    destroy();
}

void
IORegistry::setTemp(int32_t value)
{
    temp = value;
}

void
IORegistry::setLevel(int32_t value)
{
    level = value;
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
    int32_t value = 0;

    switch(type) {
        case messages::TEMP:
#ifdef POLLING_UPDATE
            value = temp;
#else
            value = get(TEMP);
#endif
            break;
        case messages::LEVEL:
#ifdef POLLING_UPDATE
            value = level;
#else
            value = get(LEVEL);
#endif
            break;
        case messages::IN_PUMP_RATE:
            value = in_pump;
            break;
        case messages::OUT_PUMP_RATE:
            value = out_pump;
            break;
        case messages::HEATER_RATE:
            value = heater;
            break;
        case messages::MIXER_RATE:
            value = mixer;
            break;
        case messages::COOLER_RATE:
            value = cooler;
            break;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            throw get_error;
    }
	return value;
}

void
IORegistry::setOutput(messages::OutputType type, int32_t value, int32_t ref)
{
    switch(type) {
        case messages::HEATER:
            heater = value;
            heater_ref = ref;
            break;
        case messages::COOLER:
            cooler = value;
            cooler_ref = ref;
            break;
        case messages::IN_PUMP:
            in_pump = value;
            in_pump_ref = ref;
            break;
        case messages::OUT_PUMP:
            out_pump = value;
            out_pump_ref = ref;
            break;
        case messages::MIXER:
            mixer = value;
            mixer_ref = ref;
            break;
        default:
            std::cerr << "Got something unexpected." << std::endl;
            throw set_error;
    }

    if (set((enum set_target) type, value) < 0) {
        throw set_error;
    }
}

Sampler::Sampler(std::vector<messages::SensorType>& sensors, IORegistry& ioreg,
        tcp::socket& sock, boost::mutex& write_mutex):
    sensors(sensors), ioreg(ioreg), m_Socket(sock), write_mutex(write_mutex) {}


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

    {
        boost::lock_guard<boost::mutex> lock(write_mutex);
        out << msg;
    }
}


#ifdef POLLING_UPDATE
Poller::Poller(IORegistry& ioreg) : ioreg(ioreg)
{}

void
Poller::operator()()
{
    int32_t temp  = get(TEMP);
    int32_t level = get(LEVEL);

    if (temp < 0 || level < 0) {
        throw get_error;
    }

    {
        boost::lock_guard<boost::mutex> lock(ioreg.mutex);
        ioreg.setTemp(temp);
        ioreg.setLevel(level);
    }
}
#endif

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
        boost::mutex write_mutex;
       
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
                            Sampler(sensors, ioreg, *m_Socket, write_mutex)));
                sampler->start();
            }

            /* Send if enything to send */
            if (send.ByteSize()) {
                boost::lock_guard<boost::mutex> lock(write_mutex);
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


class KeyboardInterupt: public std::exception {
    virtual const char* what() const throw()
    {
        return "CTRL+C interrupt";
    }
} interrupt;


void
got_signal(int)
{
    throw interrupt;
}

int
main()
{
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = got_signal;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT,&sa,NULL);

    try {
        /* GNU extension, not portable */
        std::string config(program_invocation_short_name);
        config += ".ini";

        /* Parse config file */
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config, pt);

        int listenport = pt.get<int>("General.listenport");
	std::string tty(pt.get<std::string>("General.serialport"));


        /* IO monitor for batchtank process */
        IORegistry ioreg(tty);

#ifdef POLLING_UPDATE
        int polltime = pt.get<int>("General.pollingtime");
        PeriodicTask poller(polltime, Poller(ioreg));
        poller.start();
#endif

        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), listenport);
        tcp::acceptor acceptor(io_service, endpoint);

        std::cout << "Listening to port: " << listenport << std::endl;

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
