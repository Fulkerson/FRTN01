#ifndef UTILS_H
#define UTILS_H

#ifndef D
#ifdef DEBUG
#define D
#else
#define D if(0)
#endif
#endif

using namespace google::protobuf::io;
using boost::asio::ip::tcp;

template <class Message>
class MessageIstream {
    public:
        MessageIstream(tcp::socket& sock);
        MessageIstream& operator >>(Message& msg);
    private:
        boost::asio::streambuf buf;
        tcp::socket& m_Socket;
        size_t data_read;
        std::istream is;
        IstreamInputStream raw_is;
        CodedInputStream cis;
};

template <class Message>
MessageIstream<Message>::MessageIstream(tcp::socket& sock) :
    m_Socket(sock), data_read(0), is(&buf), raw_is(&is), cis(&raw_is) {}

template <class Message>
MessageIstream<Message>&
MessageIstream<Message>::operator>>(Message& msg)
{
    uint32_t msg_size = 0;

    boost::asio::streambuf::mutable_buffers_type bufs = buf.prepare(4096);
 
    /* Wrap input stream in special codedstream */
    while (true) {
        D std::cout << "Reading varint" << std::flush;
        if (data_read && cis.ReadVarint32(&msg_size)) {
            D std::cout << " and managed to read value: " << msg_size << std::endl;
            D std::cout << "Read so far from zcos: " << raw_is.ByteCount() << std::endl;
            is.clear();
            break;
        } else {
            D std::cout << " but need more data" << std::flush;
            size_t read = m_Socket.read_some(bufs);
            D std::cout << " and got " << read << " bytes of data" << std::endl;
            buf.commit(read);
            data_read += read;
            D std::cout << "Read so far: " << data_read << std::endl;
            is.clear();
        }
    }
 
    int varintsize = CodedOutputStream::VarintSize32(msg_size);
    data_read -= varintsize;
    D std::cout << "Read so far 2: " << data_read << std::endl;

    D std::cout << data_read << " < " << msg_size << " = " << (data_read < msg_size) << std::endl;
    while (data_read < msg_size) {
        /* Fetch more data */
        size_t read = m_Socket.read_some(bufs);
        buf.commit(read);
        data_read += read;
        D std::cout << "Read so far 3: " << data_read << std::endl;
        is.clear();
    }

    /* Make sure we don't use more data than we should */
    CodedInputStream::Limit msg_limit = cis.PushLimit(msg_size);
    if(!msg.ParseFromCodedStream(&cis)) {
        std::cerr << "PARSING MESSAGE FAILED" << std::endl;
    } else {
        is.clear();
        D std::cout << "Succesfully parsed Message" << std::endl;
        data_read -= msg_size;
        D std::cout << "End parsing with read: " << data_read << std::endl;
        D std::cout << "End parsing with zcos: " << raw_is.ByteCount() << std::endl;
        cis.PopLimit(msg_limit);
    }

    return *this;
}

#endif
