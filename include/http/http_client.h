#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <memory>
#include <sstream>
#include <boost/asio.hpp>

namespace http {

    class HttpClient {

        public:
        struct UrlInfo;
        class HttpResponse;

        protected:

        boost::asio::ip::tcp::socket 
        GetSocketConnection(UrlInfo urlInfo);

        static std::string
        ConstructGetRequest(UrlInfo urlInfo);
        
        bool 
        Send(boost::asio::ip::tcp::socket* socket, std::string const& data);

        std::string 
        Receive(boost::asio::ip::tcp::socket* socket);

        UrlInfo
        ParseUrl(const std::string& url);

        std::shared_ptr<HttpResponse>
        ParseHttpResponse(const std::string& data);

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

        class HttpResponse {

            public:
            std::string http_version;

            unsigned short status_code;

            std::string status_message;

            std::string location;

            std::string content_type;

            std::string response_date;

            std::string expiry_date;

            unsigned long long content_length;

            std::string data;

            friend std::ostream& operator<<(std::ostream& out, const HttpResponse& http_response);
        };

        /// HttpGet() sends a GET request to the target
        std::shared_ptr<HttpClient::HttpResponse>
        HttpGet(const std::string& url) throw ();
    };

}

#endif