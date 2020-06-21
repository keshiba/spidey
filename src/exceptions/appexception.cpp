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