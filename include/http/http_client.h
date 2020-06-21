#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <boost/asio.hpp>

namespace http {

    class HttpClient {

        protected:
        boost::asio::ip::tcp::socket 
        GetSocketConnection(std::string ip_address_str, unsigned short port);

        static std::string
        ConstructGetRequest();
        
        void 
        Send(boost::asio::ip::tcp::socket* socket, std::string const& data);

        std::string 
        Receive(boost::asio::ip::tcp::socket* socket);


        public:
        /// HttpGet() sends a GET request to the target
        void 
        HttpGet(std::string ip_address_str, unsigned short port);
    };
}

#endif