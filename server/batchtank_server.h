#ifndef SERVER_H
#define SERVER_H

#include "batchtank.pb.h"

namespace batchtank {

/* message alias for protobuf messages. */
namespace messages = batchtank_messages;

class ConnectionThread {
    public:
        ConnectionThread(std::shared_ptr<boost::asio::ip::tcp::socket>);
        void run();
        void start();
    private:
        boost::thread                 m_Thread;
        std::shared_ptr<boost::asio::ip::tcp::socket> m_Socket;
};

}

#endif
