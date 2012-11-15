#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "asio_copy_stream.h"

using namespace google::protobuf::io;

namespace batchtank {


class InputException: public std::exception {
    virtual const char* what() const throw()
    {
        return "Could not read from stream";
    }
} input_exception;


class OutputException: public std::exception {
    virtual const char* what() const throw()
    {
        return "Could write to stream";
    }
} output_exception;


template <typename Message, typename SyncWriteStream>
class MessageOutput {
    public:
        MessageOutput(SyncWriteStream&);
        MessageOutput& operator <<(Message& msg);
    private:
        SyncWriteStream& m_Socket;
};

template <typename Message, typename SyncReadStream>
class MessageInput {
    public:
        MessageInput(SyncReadStream&);
        MessageInput& operator >>(Message& msg);
    private:
        AsioInputStream<SyncReadStream> ais;
        CopyingInputStreamAdaptor cis_adp;
        CodedInputStream cis;
};

template <typename Message, typename SyncWriteStream>
MessageOutput<Message, SyncWriteStream>::MessageOutput(SyncWriteStream& out) :
    m_Socket(out) {}

template <typename Message, typename SyncWriteStream>
MessageOutput<Message, SyncWriteStream>&
MessageOutput<Message, SyncWriteStream>::operator <<(Message& msg)
{
    /* Construct output stream */
    AsioOutputStream<SyncWriteStream> aos(m_Socket);
    CopyingOutputStreamAdaptor cos_adp(&aos);
    /* CodedOutputStream will flush on destruction */
    CodedOutputStream cos(&cos_adp);
    
    cos.WriteVarint32(msg.ByteSize());
    if(!msg.SerializeToCodedStream(&cos)) {
        throw output_exception;
    }

    return *this;
}


template <typename Message, typename SyncReadStream>
MessageInput<Message, SyncReadStream>::MessageInput(SyncReadStream& in) :
    ais(in), cis_adp(&ais), cis(&cis_adp) {}


template <typename Message, typename SyncReadStream>
MessageInput<Message, SyncReadStream>&
MessageInput<Message, SyncReadStream>::operator >>(Message& msg)
{
    uint32_t msg_size;

    /* Parse delimited protobuf message */
    if(!cis.ReadVarint32(&msg_size)) {
        throw input_exception;
    }
    /* Make sure not to read beyond limit of one message */
    CodedInputStream::Limit msg_limit = cis.PushLimit(msg_size);
    if(!msg.ParseFromCodedStream(&cis)) {
        throw input_exception;
    }
            
    cis.PopLimit(msg_limit);

    return *this;
}
}
#endif
