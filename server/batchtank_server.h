#ifndef SERVER_H
#define SERVER_H

#include <google/protobuf/io/zero_copy_stream_impl.h>
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
        boost::thread m_Thread;
        std::shared_ptr<boost::asio::ip::tcp::socket> m_Socket;
};

class PeriodicTask {
    public:
        PeriodicTask(int32_t period, std::function<void()> task);
        ~PeriodicTask();
        void start();
        void stop();
    private:
        boost::thread m_Thread;
        int32_t period;
        /* Task to be executed on a periodic basis */
        std::function<void()> task;
        void execute();
        bool run;
};

/* Monitor class used to interface with the batch process. */
class IORegistry {
    public:
        /* Used for locking when writing/reading to the batch process. */
        boost::mutex mutex;


    private:
        /* Signal copies, allow plotter to read control signals etc. */
       int32_t inletpump;
};
}

#endif
