#include <exception>
#include <string>

#include "appexception.h"

appex::AppException::AppException(appex::AppExceptionType type) {

    this->exception_type = type;

}

appex::AppException::AppException(appex::AppExceptionType type, std::exception inner_exception) 
        : appex::AppException(type) {
        
    this->inner_exception = inner_exception;
    this->is_inner_exception_set = true;
}

appex::AppException::AppException(appex::AppExceptionType type, std::string const& description) 
    : AppException(type) {

    this->exception_description = description;
}
    
const char* appex::AppException::what() const throw () {

    std::string message = "Error: ";
    message += GetExceptionMessage(this->exception_type);

    if (this->is_inner_exception_set) {
        message += "\r\n";
        message += inner_exception.what();
    }

    return message.c_str();
}


/// GetExceptionMessage returns a proper error message
/// for each message type (appex::AppExceptionType)
std::string appex::AppException::GetExceptionMessage(appex::AppExceptionType type) {

    std::string exception_message = "";

    switch(type) {
        case appex::AppExceptionType::InvalidTargetIp:
            exception_message = "Target ip is invalid";
            break;
        case appex::AppExceptionType::InvalidTargetPort:
            exception_message = "Target port is invalid";
            break;
        case appex::AppExceptionType::UnreachableTargetHost:
            exception_message = "Host unreachable";
            break;
        case appex::AppExceptionType::ConnectionRefused:
            exception_message = "Connection refused by host";
            break;
        default:
            exception_message = "Generic error";
    }

    return exception_message;
}