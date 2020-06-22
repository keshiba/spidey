#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "http/http_client.h"
#include "exceptions/appexception.h"

namespace http {

    using namespace boost::asio;

    void HttpClient::HttpGet(std::string ip_address_str, unsigned short port) {

        auto socket = GetSocketConnection(ip_address_str, port);
         
        std::string get_request = ConstructGetRequest();
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

    std::string HttpClient::ConstructGetRequest() {

        std::string get_request = "GET /index HTTP/1.1\r\n\r\nHost:localhost\r\n\r\n";

        return get_request;
    }

    ip::tcp::socket HttpClient::GetSocketConnection(std::string ip_address_str, unsigned short port) {

        io_service io_service;
        ip::tcp::socket socket(io_service);

        auto ip_address = ip::address::from_string(ip_address_str);
        auto target_endpoint = ip::tcp::endpoint(ip_address, port);

        try {
            socket.connect(target_endpoint);
        }
        catch(boost::system::system_error boost_sys_err) {
            std::cout << "Unable to connect to target"
                      << std::endl
                      << boost_sys_err.what()
                      << std::endl;
            
            throw new appex::AppException(appex::AppExceptionType::UnreachableTargetHost);
        }

        return socket;
    }
}