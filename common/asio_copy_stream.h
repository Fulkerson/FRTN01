#ifndef ASIO_COPY_STREAM
#define ASIO_COPY_STREAM

/*!
 *  \file asio_copy_stream.h
 *
 *  Implementations of google::protobuf::io::CopyingInputStream and
 *  google::protobuf::io::CopyingOutputStream for use with the
 *  synchronized parts of boost::asio.
 */

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

//#define DEBUG_UTILS_H

using namespace google::protobuf::io;
using boost::asio::ip::tcp;

namespace batchtank {

/*!
 * Used together with google::protobuf::io::CopyingInputStreamAdaptor
 * to create a ZeroCopyInputStream using a synchronous read stream:
 * http://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/reference/SyncReadStream.html
 */
template <typename SyncReadStream>
class AsioInputStream : public CopyingInputStream {
    public:
        AsioInputStream(SyncReadStream& sock);
        int Read(void* buffer, int size);
    private:
        SyncReadStream& m_Socket;
};


/*!
 * Used together with google::protobuf::io::CopyingOutputStreamAdaptor
 * to create a ZeroCopyOutputStream using a synchronous write stream:
 * http://www.boost.org/doc/libs/1_50_0/doc/html/boost_asio/reference/SyncWriteStream.html
 */
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

    std::size_t bytes_read;
    boost::system::error_code ec;
    bytes_read = m_Socket.read_some(boost::asio::buffer(buffer, size), ec);

#ifdef DEBUG_UTILS_H
    std::cout << " of which " << bytes_read << " was read." << std::endl;
#endif

    if(!ec) {
        return bytes_read;
    } else if (ec == boost::asio::error::eof) {
        return 0;
    } else {
        return -1;
    }
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
    
    std::size_t bytes_written;
    boost::system::error_code ec;
    bytes_written = m_Socket.write_some(boost::asio::buffer(buffer, size), ec);

#ifdef DEBUG_UTILS_H
    std::cout << " of which " << bytes_written << " was written." << std::endl;
#else 
    /* Supress unused variable warning */
    (void)bytes_written;
#endif

    return !ec;
}

}

#endif
