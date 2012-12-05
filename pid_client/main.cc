
#include <string>
#include <csignal>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "../common/batchtank.pb.h"
#include "../common/message_utils.h"
#include <control/pid.h>

namespace batchtank
{
namespace messages = batchtank_messages;
}

using namespace batchtank;
static bool do_update = true;

/* Set update */
void got_signal(int)
{
	do_update = true;
}

class Parameters
{
public:
	PIDParameters pp;
	int period;

	Parameters(std::string config, int period);

	// Return true if parameters were updated.
	bool update_parameters();


private:
	boost::property_tree::ptree pt;
	std::string config;
	time_t last_change;
};

bool Parameters::update_parameters()
{
	// Check when .ini was last modified.
	if (do_update) {
		boost::property_tree::ini_parser::read_ini(config, pt);
		pp = PIDParameters(
			     pt.get<double>("PID.K"),
			     pt.get<double>("PID.Ti"),
			     pt.get<double>("PID.Td"),
			     pt.get<double>("PID.Tr"),
				 (double) period,
			     pt.get<double>("PID.ref"),
			     pt.get<double>("PID.umin"),
			     pt.get<double>("PID.umax"),
			     pt.get<bool>("PID.inverted")
		     );
		do_update = false;
		return true;
	} else {
		return false;
	}
}

std::ostream& operator <<(std::ostream& os, const Parameters& p)
{
	os << p.pp;
	return os;
}

Parameters::Parameters(std::string config, int period) :
		period(period), config(config), last_change(0) {}


int main(int argc, char* argv[])
{

	/* Set user defined signal for updating parameters */
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa)); 
	sa.sa_handler = got_signal;
	sa.sa_flags = SA_RESTART;
	sigfillset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);

	// In some uses argv[0] might not contain executable name.
	if (argc < 1) {
		std::cerr << "No implicit name given on cmdline" << std::endl;
		return 1;
	}

	std::string config;
	if (argc == 2) {
		// Config given on commandline
		config = argv[1];
	} else {
		// Use executablename.ini as config
		config = argv[0];
		config += ".ini";
	}

	// Parse config file
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(config, pt);

	std::string ip(pt.get<std::string>("General.ipaddress"));
	int port = pt.get<int>("General.port");
	int period = pt.get<int>("General.period");
	int delay = pt.get<int>("General.delay");

	messages::Sensor sensor;

	std::string sensor_(pt.get<std::string>("General.sensor"));

	if (sensor_ == "TEMP") {
		sensor = messages::TEMP;
	} else if (sensor_ == "LEVEL") {
		sensor = messages::LEVEL;
	} else if (sensor_ == "IN PUMP") {
		sensor = messages::IN_PUMP_RATE;
	} else if (sensor_ == "OUT PUMP") {
		sensor = messages::OUT_PUMP_RATE;
	} else if (sensor_ == "HEATER") {
		sensor = messages::HEATER_RATE;
	} else if (sensor_ == "COOLER") {
		sensor = messages::COOLER_RATE;
	} else if (sensor_ == "MIXER") {
		sensor = messages::MIXER_RATE;
	} else {
		std::cerr << "Invalid sensor";
		return 1;
	}

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
		return 1;
	}

	// Setup a parameter parser.
	Parameters parameters(config, period);

	PID pid;

	try {
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
		messages::Register* reg = msg.mutable_register_();
		reg->set_periodtime(period);
		reg->add_type(sensor);

		// Send message
		out << msg;

		// At the time of creation blocking calls are made so we must
		// place this after registration is set up properly
		MessageInput<messages::BaseMessage, tcp::socket> input(socket);

		while (true) {

			// Update parameters if needed.
			if (parameters.update_parameters()) {
				pid.updateParameters(parameters.pp);
				std::cout << "New parameters:" << std::endl;
				std::cout << parameters << std::endl;
			}

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
			if (s.type() != sensor) {
				std::cerr << "Didn't receive proper message" << std::endl;
				break;
			} else {
#ifdef DEBUG
				std::cout << "Received: " << msg.DebugString() << std::endl;
#endif
			}

			double value = s.value();

			// Do stuff with sample data
			value = pid.next(value);

			// Prepare for send
			msg.Clear();


			// Simulate delay
			sleep((double)rand()/RAND_MAX*delay/1000);	

			// Create control signal
			messages::ControlSignal* sig = msg.add_signal();
			sig->set_type(output);
			sig->set_value(value);
			sig->set_ref(parameters.pp.r);
			out << msg;

			pid.updateStates();
		}
	} catch (std::exception& e)  {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}



