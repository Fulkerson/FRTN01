#ifndef SERVER_H
#define SERVER_H

class Connection {
    public:
        Connection(boost::asio::ip::tcp::socket*);
        void run();
        ~Connection();
    private:
        boost::asio::ip::tcp::socket* socket;
};

#endif
