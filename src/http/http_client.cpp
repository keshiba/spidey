#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <regex>
#include <memory>
#include <sstream>
#include <cstdlib>
#include <string>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "http/http_client.h"
#include "exceptions/appexception.h"

namespace http {

    using namespace boost::asio;

    std::shared_ptr<HttpClient::HttpResponse> HttpClient::HttpGet(const std::string& url) throw () {

        unsigned int redirection_counter = 100;
        std::shared_ptr<HttpClient::HttpResponse> http_response;
        HttpClient::UrlInfo urlInfo = ParseUrl(url);
        std::unordered_set<std::string> redirection_history;
        redirection_history.insert(url);

        while (redirection_counter-- > 0) {

            auto socket = GetSocketConnection(urlInfo);
            auto get_request = ConstructGetRequest(urlInfo);

            bool is_send_successful = this->Send(&socket, get_request);
            if (is_send_successful) {

                auto data = this->Receive(&socket); 

                http_response = ParseHttpResponse(data);
                if (http_response->status_code < 300 || http_response->status_code > 310) {
                    break;
                }
                else {

                    if (redirection_history.find(http_response->location) != redirection_history.end()) {
                        throw new appex::AppException(appex::AppExceptionType::RedirectionLoopDetected);
                    }
                    else {
                        redirection_history.insert(http_response->location);
                    }

                    urlInfo = ParseUrl(http_response->location);
                    std::cout << "Redirecting to " << http_response->location << std::endl;
                }
            }
            else {
                throw new appex::AppException(appex::AppExceptionType::SendDataFailed);
            }
        }

        if (redirection_counter == 0) {
            throw new appex::AppException(appex::AppExceptionType::RedirectionLimitExceeded);
        }

        return http_response;
    }

    std::shared_ptr<HttpClient::HttpResponse> HttpClient::ParseHttpResponse(const std::string& data) {

        typedef boost::split_iterator<std::string::const_iterator> split_iterator;
        auto http_response = std::make_shared<HttpClient::HttpResponse>();
        std::string delim("\r\n");
        bool parse_body = false;

        split_iterator it = 
            boost::make_split_iterator(data, boost::first_finder(delim, boost::is_equal()));
        
        std::string status_line = boost::copy_range<std::string>(*it);
        std::size_t http_status_pos = status_line.find(" ");
        std::string http_status_str = status_line.substr(0, http_status_pos);
        http_response->http_version = http_status_str;

        std::size_t status_code_pos = status_line.find(" ", http_status_str.length() + 1);
        std::string status_code_str = status_line.substr(http_status_pos + 1, status_code_pos);
        http_response->status_code = strtoul(status_code_str.c_str(), NULL, 0);
        http_response->status_message = status_line.substr(status_code_pos + 1, std::string::npos);

        std::string body_str;

        ++it;
        for(; it != split_iterator(); ++it) {

            std::string data_line = boost::copy_range<std::string>(*it);

            if (data_line == "") {
                parse_body = true;
            }

            if (parse_body) {
                body_str += data_line;
            }
            else {
                std::size_t key_pos = data_line.find_first_of(":");

                if (key_pos != std::string::npos) {
                    std::string key = data_line.substr(0, key_pos);
                    std::string value = data_line.substr(key_pos + 1, std::string::npos);
                    boost::trim(key);
                    boost::trim(value);

                    if (key == "Location") {
                        http_response->location = value;
                    }
                    else if (key == "Content-Type") {
                        http_response->content_type = value;
                    }
                    else if (key == "Date") {
                        http_response->response_date = value;
                    }
                    else if (key == "Expires") {
                        http_response->expiry_date = value;
                    }
                    else if (key == "Content-Length") {
                        http_response->content_length = strtoull(value.c_str(), NULL, 0);
                    }
                }
            }
        }

        http_response->data = body_str;

        return http_response;
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

    bool HttpClient::Send(ip::tcp::socket* socket, std::string const& data) {

        boost::system::error_code error;
        auto send_buffer = boost::asio::buffer(data);

        std::cout << "Request: " << std::endl
                  << data << std::endl;

        boost::asio::write(*socket, send_buffer, error);

        if (error) {
            std::cout << "Send failed: " 
                      << error.message()
                      << std::endl;

            return false;
        }

        return true;
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

    HttpClient::UrlInfo HttpClient::ParseUrl(const std::string& url) {
        
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

    std::ostream& operator<<(std::ostream& out, const HttpClient::HttpResponse& http_response) {
        out << "HttpResponse : {" << std::endl
            << "\tHttp-Version: " << http_response.http_version << std::endl
            << "\tStatus: " << http_response.status_code << std::endl
            << "\tStatus-Message: " << http_response.status_message << std::endl
            << "\tLocation: " << http_response.location << std::endl
            << "\tContent-Type: " << http_response.content_type << std::endl
            << "\tContent-Length: " << http_response.content_length << std::endl
            << "\tDate: " << http_response.response_date << std::endl
            << "\tExpires: " << http_response.expiry_date << std::endl
            << "\tData: " << http_response.data << std::endl
            << "}" << std::endl;
    }
}