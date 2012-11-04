#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "batchtank_server.h"

using boost::asio::ip::tcp;
using namespace batchtank;


ConnectionThread::ConnectionThread(std::shared_ptr<tcp::socket> socket) :
    m_Socket(socket) {}

   
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
        std::string message("Hello World!\n");
        boost::system::error_code ignored_error;

        while(true) {
            boost::asio::write(*m_Socket, boost::asio::buffer(message),
                               ignored_error);
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
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 54000));

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
