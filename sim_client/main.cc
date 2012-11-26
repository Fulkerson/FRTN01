
#include <string>
#include <ctime>
#include <sys/stat.h>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "batchtank.pb.h"
#include "../common/message_utils.h"

namespace batchtank {
	namespace messages = batchtank_messages;
}

using namespace batchtank;

int main(int argc, char* argv[]) {
	// In some uses argv[0] might not contain executable name.
	if (argc < 1) {
		std::cerr << "No implicit name given on cmdline" << std::endl;
		return 1;
	}
	// Use executablename.ini as config
        std::string config(argv[0]);
        config += ".ini";

        // Parse config file
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config, pt);

	std::string ip(pt.get<std::string>("General.ipaddress"));
        int port = pt.get<int>("General.port");

	messages::Output output;
	std::string output_(pt.get<std::string>("General.output"));
	
	if (output_ == "HEATER") {
		output = messages::HEATER;
	} else if (output_ == "COOLER") {
		output = messages::COOLER;
	} else if (output_ == "IN PUMP") {
		output = messages::IN_PUMP;
	} else if (output_ == "OUT PUMP") {
		output = messages::OUT_PUMP;
	} else if (output_ == "MIXER") {
		output = messages::MIXER;
	} else {
		std::cerr << "Invalid output" << std::endl;
	}

    try {
		std::cout << "whey\n";
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::address::from_string(ip), port);

        tcp::socket socket(io_service);
        socket.connect(endpoint);
        std::cout << "Connected" << std::endl;

        // Socket to protobuf glue
        MessageOutput<messages::BaseMessage, tcp::socket> out(socket);

        // First register for sensor data
        messages::BaseMessage msg;

        while (true) {
			double value = 100;

            // Prepare for send 
            msg.Clear();

            // Create control signal
            messages::ControlSignal* sig = msg.add_signal();
            sig->set_type(output);
            sig->set_value(value);
            sig->set_ref(value);
            out << msg;
			sleep(pt.get<int>("General.period") / 1000);
        }
    } catch (std::exception& e)  {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}



