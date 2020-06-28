#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <boost/asio.hpp>

namespace http {

    class HttpClient {

        public:
        struct UrlInfo;

        protected:

        boost::asio::ip::tcp::socket 
        GetSocketConnection(UrlInfo urlInfo);

        static std::string
        ConstructGetRequest(UrlInfo urlInfo);
        
        void 
        Send(boost::asio::ip::tcp::socket* socket, std::string const& data);

        std::string 
        Receive(boost::asio::ip::tcp::socket* socket);

        UrlInfo
        ParseUrl(std::string url);

        std::string
        ResolveNameToIp(const std::string& host_name, const std::string& protocol);

        private:
        boost::asio::io_service io_service;

        public:

        struct UrlInfo {

            std::string hostname; 

            std::string host_ip;

            unsigned short port;

            std::string service;

            std::string resource;

            bool is_valid;
        };

        /// HttpGet() sends a GET request to the target
        void 
        HttpGet(std::string url) throw ();
    };

}

#endif