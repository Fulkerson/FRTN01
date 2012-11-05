#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "batchtank_server.h"

using boost::asio::ip::tcp;
using namespace batchtank;


ConnectionThread::ConnectionThread(std::shared_ptr<tcp::iostream> stream) :
    m_Stream(stream) {}

   
void
ConnectionThread::start()
{
    m_Thread = boost::thread(&ConnectionThread::run, this);
    m_Thread.detach();
}


void
ConnectionThread::run()
{
    try {
        boost::posix_time::seconds periodTime(3);
        boost::system::error_code ignored_error;

        while(true) {
            std::cout << "hello world!" << std::endl;
            *m_Stream << "hello world!" << std::endl;
            boost::this_thread::sleep(periodTime);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
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
            std::shared_ptr<tcp::iostream> stream(new tcp::iostream());
            acceptor.accept(*(stream->rdbuf()));

            (new ConnectionThread(stream))->start();

        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
