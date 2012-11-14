#ifndef UTILS_H
#define UTILS_H
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

//#define DEBUG_UTILS_H

using namespace google::protobuf::io;
using boost::asio::ip::tcp;

template <typename SyncReadStream>
class AsioInputStream : public CopyingInputStream {
    public:
        AsioInputStream(SyncReadStream& sock);
        int Read(void* buffer, int size);
    private:
        SyncReadStream& m_Socket;
};

template <typename SyncWriteStream>
class AsioOutputStream : public CopyingOutputStream {
    public:
        AsioOutputStream(SyncWriteStream& sock);
        bool Write(const void* buffer, int size);
    private:
        SyncWriteStream& m_Socket;
};

template <typename SyncReadStream>
AsioInputStream<SyncReadStream>::AsioInputStream(SyncReadStream& sock) :
    m_Socket(sock) {}

template <typename SyncReadStream>
int
AsioInputStream<SyncReadStream>::Read(void* buffer, int size)
{
#ifdef DEBUG_UTILS_H
    std::cout << "DEBUG: Trying to read " << size << " bytes" << std::flush;
#endif

    std::size_t nbr_read;
    nbr_read = m_Socket.read_some(boost::asio::buffer(buffer, size));

#ifdef DEBUG_UTILS_H
    std::cout << " of which " << nbr_read << " was read." << std::endl;
#endif

    return nbr_read;
}

template <typename SyncWriteStream>
AsioOutputStream<SyncWriteStream>::AsioOutputStream(SyncWriteStream& sock) :
    m_Socket(sock) {}

template <typename SyncWriteStream>
bool
AsioOutputStream<SyncWriteStream>::Write(const void* buffer, int size)
{
#ifdef DEBUG_UTILS_H
    std::cout << "DEBUG: Trying to write " << size << " bytes" << std::flush;
#endif
    
    std::size_t nbr_written;
    nbr_written = m_Socket.write_some(boost::asio::buffer(buffer, size));

#ifdef DEBUG_UTILS_H
    std::cout << " of which " << nbr_written << " was written." << std::endl;
#endif

    return !size || nbr_written != 0;
}

#endif
