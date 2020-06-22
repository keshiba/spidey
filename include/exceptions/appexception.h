#ifndef APPEXCEPTION_H
#define APPEXCEPTION_H


namespace appex {

    /// AppExceptionType enum holds all the high-level exceptions
    /// that occur in the application which will be notified back
    /// to the user
    enum AppExceptionType {

        InvalidTargetIp = 1,
        InvalidTargetPort = 2,
        UnreachableTargetHost = 3,
        UnreachableTargetPort = 4
    };

    /// AppException is a generic exception container
    /// to represent all high-level exceptions occuring in the
    /// application which will be given back to the user
    /// a pretty-printed exception message
    class AppException: public std::exception {

        protected:

        AppExceptionType exception_type;
        std::string exception_description;
        bool is_inner_exception_set = false;
        std::exception inner_exception;


        public:

        AppException(AppExceptionType type);
        AppException(AppExceptionType type, std::exception inner_exception);
        AppException(AppExceptionType type, std::string const& description);
        
        /// what() constucts an error description for this
        /// particular exception
        const char* 
        what() const throw();

        static std::string 
        GetExceptionMessage(appex::AppExceptionType type);
    };
}

#endif