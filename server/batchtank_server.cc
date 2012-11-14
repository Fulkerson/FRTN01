#include <iostream>
#include <string>
#include <time.h>
#include <cerrno>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#define DEBUG

#ifndef D
#ifdef DEBUG
#define D
#else
#define D if(0)
#endif
#endif

#include "batchtank_server.h"
#include "utils.h"

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


ConnectionThread::ConnectionThread(std::shared_ptr<tcp::socket> m_Socket) :
    m_Socket(m_Socket) {}


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
        boost::system::error_code ignored_error;

        boost::asio::streambuf obuf;
        std::ostream os(&obuf);

        AsioInputStream<tcp::socket> ais(*m_Socket);
        google::protobuf::io::CopyingInputStreamAdaptor cis_adaptor(&ais);
        google::protobuf::io::CodedInputStream cis(&cis_adaptor);
       
        PeriodicTask periodic(100, [this, &obuf, &os]() {
                google::protobuf::io::OstreamOutputStream raw_os(&os);
                google::protobuf::io::CodedOutputStream cos(&raw_os);
                cos.WriteVarint64(100);
                boost::asio::write(*m_Socket, obuf);
            });
        //periodic.start();

        messages::BaseMessage msg;
        uint32_t msg_size;

        while (true) {
            /* Parse delimited protobuf message */
            cis.ReadVarint32(&msg_size);
            CodedInputStream::Limit msg_limit = cis.PushLimit(msg_size);
            msg.ParseFromCodedStream(&cis);
            
            cis.PopLimit(msg_limit);
            D std::cout << "DEBUG: " << msg.DebugString() << std::endl;

            if (msg.has_endconnection() && msg.endconnection()) {
                std::cerr << "Client: " << connected_to
                    << " asks to gracefully end connection" << std::endl;
                break;
            }

            msg.Clear();
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
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 54000);
        tcp::acceptor acceptor(io_service, endpoint);

        while(true) {
            std::shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
            acceptor.accept(*socket);

            (new ConnectionThread(socket))->start();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
