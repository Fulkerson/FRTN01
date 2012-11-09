#ifndef UTILS_H
#define UTILS_H

#define UNUSED(expr) if(0) { (void)(expr); }

using namespace google::protobuf::io;

class MessageParser {
    public:
        MessageParser(batchtank_messages::BaseMessage& msg,
                boost::asio::streambuf& buf);
        bool operator()(const boost::system::error_code& error,
                std::size_t bytes_transferred);
    private:
        bool size_parsed;
        std::size_t nbr_read;
        uint32_t msg_size;
        batchtank_messages::BaseMessage& msg;
        boost::asio::streambuf& buf;
};

MessageParser::MessageParser(batchtank_messages::BaseMessage& msg,
        boost::asio::streambuf& buf) : 
    size_parsed(false),
    nbr_read(0),
    msg_size(0),
    msg(msg),
    buf(buf) {}


bool
MessageParser::operator()(const boost::system::error_code& error,
        std::size_t bytes_transferred)
{
    /* TODO: Parse error condition. */
    UNUSED(error);

    /* Wrap input stream in special codedstream */
    std::istream is(&buf);
    IstreamInputStream raw_is(&is);
    CodedInputStream cis(&raw_is);

    if (size_parsed || (size_parsed = cis.ReadVarint32(&msg_size))) {
        /* Message size parsed successfully */

        /* Make sure we don't use more data than we should */
        CodedInputStream::Limit msg_limit = cis.PushLimit(msg_size);

        uint32_t left = bytes_transferred - CodedOutputStream::VarintSize32(msg_size);

        if (left < msg_size) {
            /* Not enough data to finish parsing */
            cis.PopLimit(msg_limit);
            return false;
        } else {
            /* Try to parse message */
            if(!msg.ParseFromCodedStream(&cis)) {
                std::cerr << "PARSING MESSAGE FAILED" << std::endl;
            }

            cis.PopLimit(msg_limit);
            return true;
        }
    } else {
        /* Need more data to parse message size */
        return false;
    }
 
}

#endif
