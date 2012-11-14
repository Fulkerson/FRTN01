
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>

class Connection{
	private:
		int derp;
	public:
		Connection();
		void setUp(const std::string,int);
		bool isUp();
		double sendAndReceive(double);
};
#endif
