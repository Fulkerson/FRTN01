#ifndef UTILS_H
#define UTILS_H

#ifndef D
#ifdef DEBUG
#define D
#else
#define D if(0)
#endif
#endif

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

using namespace google::protobuf::io;
using boost::asio::ip::tcp;

template <class SocketService>
class AsioInputStream : public CopyingInputStream {
    public:
        AsioInputStream(SocketService& sock);
        ~AsioInputStream();
        int Read(void* buffer, int size);
        int Skip(int count);
    private:
        SocketService& m_Socket;
};

template <class SocketService>
AsioInputStream<SocketService>::AsioInputStream(SocketService& sock) :
    m_Socket(sock) {}

template <class SocketService>
AsioInputStream<SocketService>::~AsioInputStream() {}

template <class SocketService>
int
AsioInputStream<SocketService>::Read(void* buffer, int size)
{
    std::size_t nbr_read;
    D std::cout << "INFO: Trying to read " << size << " bytes" << std::flush;
    nbr_read = m_Socket.read_some(boost::asio::buffer(buffer, size));
    D std::cout << " of which " << nbr_read << " was read." << std::endl;
    
    return nbr_read;
}

template <class SocketService>
int
AsioInputStream<SocketService>::Skip(int count)
{
    std::size_t nbr_read = 0;
    std::cerr << "WARNING: Trying to skip " << count << " bytes" << std::flush;
    //nbr_read = m_Socket.read_some(boost::asio::buffer(NULL, count));
    std::cerr << " of which " << nbr_read << " was skipped." << std::endl;
    return nbr_read;
}

#endif
