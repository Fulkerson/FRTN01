#ifndef SERVER_H
#define SERVER_H

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "batchtank.pb.h"

namespace batchtank {

/* message alias for protobuf messages. */
namespace messages {
    using namespace batchtank_messages;
    /* TODO: I do not understand why Sensor is a pointer... */
    typedef int SensorType;
    typedef int OutputType;

}


/* Monitor class used to interface with the batch process. */
class IORegistry {
    public:
        IORegistry();
        /* Used for locking when writing/reading to the batch process. */
        boost::mutex mutex;
        double getSensor(messages::SensorType);
        double getOutput(messages::OutputType);
        void setOutput(messages::OutputType, double value, double ref);
    private:
        /* Signal copies, allow plotter to read control signals etc. */
        int32_t heater;
        int32_t heater_ref;
        int32_t cooler;
        int32_t cooler_ref;
        int32_t in_pump;
        int32_t in_pump_ref;
        int32_t out_pump;
        int32_t out_pump_ref;
        int32_t mixer;
        int32_t mixer_ref;
};


class ConnectionThread {
    public:
        ConnectionThread(std::shared_ptr<boost::asio::ip::tcp::socket>,
                IORegistry&);
        ~ConnectionThread();
        void run();
        void start();
    private:
        boost::thread m_Thread;
        std::shared_ptr<boost::asio::ip::tcp::socket> m_Socket;
        IORegistry& ioreg;
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

/* Function object used for sampling */
class Sampler {
    public:
        Sampler(std::vector<messages::SensorType>&, IORegistry&,
                boost::asio::ip::tcp::socket&);
        void operator()();
    private:
        messages::BaseMessage msg;
        std::vector<messages::SensorType> sensors;
        IORegistry& ioreg;
        boost::asio::ip::tcp::socket& m_Socket;
};

}

#endif
