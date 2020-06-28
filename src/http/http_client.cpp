#include <iostream>
#include <algorithm>
#include <regex>
#include <memory>
#include <sstream>
#include <cstdlib>
#include <boost/asio.hpp>
#include "http/http_client.h"
#include "exceptions/appexception.h"

namespace http {

    using namespace boost::asio;

    void HttpClient::HttpGet(std::string url) throw () {

        auto urlInfo = ParseUrl(url);
        auto socket = GetSocketConnection(urlInfo);
        auto get_request = ConstructGetRequest(urlInfo);

        this->Send(&socket, get_request);

        auto data = this->Receive(&socket); 

        std::cout << data.length() << " characters received" << std::endl;
        std::cout << data << std::endl;
    }

    std::string HttpClient::Receive(ip::tcp::socket* socket) {

        boost::system::error_code error;
        streambuf receive_buffer;
        std::string receive_str = "";

        boost::asio::read(*socket, receive_buffer, boost::asio::transfer_all(), error);

        if (error && error != error::eof) {
            std::cout << "Receive failed: "
                      << error.message()
                      << std::endl;
        }
        else {
            std::cout << "Message received"
                      << std::endl;
            
            auto recv_data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
            receive_str = std::string(recv_data);
        }

        return receive_str;
    }

    void HttpClient::Send(ip::tcp::socket* socket, std::string const& data) {

        boost::system::error_code error;
        auto send_buffer = boost::asio::buffer(data);

        std::cout << "Request: " << std::endl
                  << data << std::endl;

        boost::asio::write(*socket, send_buffer, error);

        if (error) {
            std::cout << "Send failed: " 
                    << error.message()
                    << std::endl;
        }
        else {
            std::cout << "Sent message successfully"
                    << std::endl;
        }
    }

    std::string HttpClient::ConstructGetRequest(UrlInfo urlInfo) {

        std::stringstream request_generator;

        request_generator << "GET " << urlInfo.resource << " HTTP/1.0" << "\r\n"
                          << "Host: " << urlInfo.hostname << "\r\n\r\n";
                          
        return request_generator.str();
    }

    ip::tcp::socket HttpClient::GetSocketConnection(UrlInfo urlInfo) {

        auto ip_address_str = ResolveNameToIp(urlInfo.hostname, urlInfo.service);
        urlInfo.host_ip = ip_address_str;

        auto ip_address = ip::address::from_string(urlInfo.host_ip);
        unsigned short port = urlInfo.port;
        auto target_endpoint = ip::tcp::endpoint(ip_address, port);
        ip::tcp::socket socket(this->io_service);

        try {
            socket.connect(target_endpoint);
        }
        catch(boost::system::system_error boost_sys_err) {
            
            std::cout << boost_sys_err.what() << std::endl;
            throw new appex::AppException(appex::AppExceptionType::UnreachableTargetHost);
        }

        return socket;
    }

    HttpClient::UrlInfo HttpClient::ParseUrl(std::string url) {
        
        std::smatch match;
        std::regex url_regex("^((http[s]?):\\/\\/)?([a-zA-Z0-9-.]+)(:(\\d+))?(\\/(.*))?$");
        UrlInfo urlInfo;

        if (std::regex_search(url, match, url_regex)) {

            std::string hostname = match[3];
            urlInfo.hostname = (hostname == "") ? "localhost" : hostname;
            
            unsigned short port = strtoul(match[5].str().c_str(), NULL, 0);
            urlInfo.port = (port == 0) ? 80 : port;

            std::string resource = match[6];
            urlInfo.resource = (resource == "") ? "/" : resource;
            
            std::string service_str = match[2].str();
            if (service_str != "") {
                std::transform(service_str.begin(), service_str.end(), 
                                service_str.begin(), ::tolower);
            }
            else {
                service_str = "http";
            }
            urlInfo.service = service_str;

            urlInfo.is_valid = true;
        }

        return urlInfo;
    }

    std::string HttpClient::ResolveNameToIp(const std::string& host_name, const std::string& service) {

        auto protocol = ip::tcp::v4();
        ip::tcp::resolver name_resolver(this->io_service);
        ip::tcp::resolver::query query(protocol, host_name, service, ip::tcp::resolver::query::canonical_name);
        boost::system::error_code resolution_error;
        std::string ip_address_str;

        ip::tcp::resolver::iterator ip_resolve_iter = name_resolver.resolve(query, resolution_error); 

        if (resolution_error) {
            std::cout << "Unable to resolve name to ip: "
                      << resolution_error.message()
                      << std::endl;
        }
        else {
            boost::system::error_code address_error;

            ip_address_str = ip_resolve_iter->endpoint().address().to_v4().to_string(address_error);

            if (address_error) {
                std::cout << "Unable to convert address to string"
                            << address_error.message()
                            << std::endl;
            }
        }

        return ip_address_str;
    }
}