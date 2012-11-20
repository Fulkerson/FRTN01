
#include <string>

#include "batchtank.pb.h"
#include <boost/asio.hpp>
#include "../common/message_utils.h"
#include <control/pid.h>


namespace batchtank {
	namespace messages = batchtank_messages;
}

using namespace batchtank;


// args: ip port period K Ti Td yref min max
int main(int argc, const char* argv[]) {

	// Parse some args
	std::string ip("127.0.0.1");
	int port = 54000;
	int period = 50;
	messages::Sensor sensor = messages::HEATERSENSOR;
	
	double pp[] = {1,2,1,2,-5,5};	

	if (argc == 2 && atoi(argv[1]) == 0) {
		std::cout << "Running on standard params\n";
	} else if (argc == 11) {
		ip = std::string(argv[1]);
		port = atoi(argv[2]);
		std::string sens(argv[3]);
		period = atoi(argv[4]);

		if (sens == "HEAT")
			sensor = messages::HEATERSENSOR;
		else if (sens == "COOL")
			sensor = messages::COOLERSENSOR;
		else if (sens == "IN")
			sensor = messages::INLETSENSOR;
		else if (sens == "OUT")
			sensor = messages::OUTLETSENSOR;

		for(int i = 0; i < 6; i++)
			pp[i] = atoi(argv[i+5]);
		std::cout << "Parsed command line parameters:\n";
	} else {
		std::cout << "Usage: ./client ip port sensor period"
			<< " K Ti Td yref umin umax\n"
			<< "\tsensor = HEAT|COOL|IN|OUT\n";
		return 0;	
	}
	
	std::cout << "\t" << ip << ":" << port << "\t" << period << "\n";
	for(int i = 0; i < 6; i++)
		std::cout << "\t" << pp[i];
	std::cout << "\n";
	
	PID* pid = new PID(pp[0],pp[1],pp[2],pp[3],pp[4],pp[5]);	
	
    try {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::address::from_string(ip), port);

        tcp::socket socket(io_service);
        socket.connect(endpoint);
        std::cout << "Connected" << std::endl;

        // Socket to protobuf glue
        MessageOutput<messages::BaseMessage, tcp::socket> output(socket);

        // First register for sensor data
        messages::BaseMessage msg;
        messages::Register* reg = msg.mutable_register_();
        reg->set_periodtime(period);
        reg->add_type(sensor);
        
        // Send message 
        output << msg;

        // At the time of creation blocking calls are made so we must
        // place this after registration is set up properly 
        MessageInput<messages::BaseMessage, tcp::socket> input(socket);

        while (true) {
            // Reusing messages are faster 
            msg.Clear();

            // Read message 
            input >> msg;

            // See if there's any samples, there should be exactly one.
            if (msg.sample_size() != 1) {
                std::cerr << "Got incorrect number of samples!" << std::endl;
                break;
            }

            // Get sample 
            messages::Sample s = msg.sample(0);
            if (s.type() != messages::HEATERSENSOR) {
                std::cerr << "Didn't receive proper message" << std::endl;
                break;
            } else {
                std::cout << "Received: " << msg.DebugString() << std::endl;
            }
            
            double value = s.value();

            // Do stuff with sample data
			value = pid->next(value);

            // Prepare for send 
            msg.Clear();

            // Create control signal
            messages::ControlSignal* sig = msg.add_signal();
            sig->set_type(messages::HEATER);
            sig->set_value(value);
            output << msg;
        }
    } catch (std::exception& e)  {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}



