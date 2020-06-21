#ifndef APP_H
#define APP_H

namespace spidey {

    /// CliArgs holds the arguments passed when invoking the application
    struct CliArgs {

        std::string target_ip;

        unsigned short target_port;

        bool is_parse_successful;

        std::string parse_error;
    };

    /// spidey::Start initiates crawling links on the target
    /// provided
    int Start(CliArgs args);
}

#endif