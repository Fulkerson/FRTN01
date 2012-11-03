#include <iostream>
#include <thread>

using namespace std;

void
hello()
{
    cout << "Hello from thread " << endl;
}

int
main()
{
    thread t1(hello);

    t1.join();

    return 0;
}
