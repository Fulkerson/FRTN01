
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/condition_variable.hpp>


class Connection{
	private:
		bool up;
		double in;
		double out;
		double isIn;
		double isOut;
		boost::condition_variable cond;
		boost::mutex mutex;
		double getSend();
		void putReceive();
		void work();
	public:
		Connection(std::string url, int port);
		void run();
		void send(double);
		double receive();
}
