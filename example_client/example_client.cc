#include <string>
#include <boost/asio.hpp>

#include "example_client.h"
#include "../common/message_utils.h"

using boost::asio::ip::tcp;
using namespace batchtank;

int
main()
{
    try {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), 54000);

        tcp::socket socket(io_service);
        socket.connect(endpoint);
        std::cout << "Connected" << std::endl;

        /* Socket to protobuf glue */
        MessageOutput<messages::BaseMessage, tcp::socket> output(socket);

        /* First register for sensor data */
        messages::BaseMessage msg;
        messages::Register* reg = msg.mutable_register_();
        reg->set_periodtime(50);
        reg->add_type(messages::HEATER_RATE);
        
        /* Send message */
        output << msg;

        /* At the time of creation blocking calls are made so we must
         * place this after registration is set up properly */
        MessageInput<messages::BaseMessage, tcp::socket> input(socket);

        while (true) {
            /* Reusing messages are faster */
            msg.Clear();

            /* Read message */
            input >> msg;

            /* See if there's any samples, there should be exactly one. */
            if (msg.sample_size() != 1) {
                std::cerr << "Got incorrect number of samples!" << std::endl;
                break;
            }

            /* Get sample */
            messages::Sample s = msg.sample(0);
            if (s.type() != messages::HEATER_RATE) {
                std::cerr << "Didn't receive proper message" << std::endl;
                break;
            } else {
                std::cout << "Received message" <<  std::endl;
            }
            
            double value = s.value();

            // Do stuff with sample data
            
            /* Prepare for send */
            msg.Clear();

            /* Create control signal */
            messages::ControlSignal* sig = msg.add_signal();
            sig->set_type(messages::HEATER);
            sig->set_value(value * 2);
            output << msg;
        }
    } catch (std::exception& e)  {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}



