#include <iostream>
#include <exception>
#include <string.h>
#include <cstdlib>
#include <exception>
#include "app.h"

spidey::CliArgs ParseCliArgs(int argc, char** argv) {

    spidey::CliArgs args;

    if (argc != 3) {
        args.is_parse_successful = false;
        args.parse_error = "missing arguments";
        
        return args;
    }

    size_t ip_arg_len = strlen(argv[1]);
    if (ip_arg_len == 0 || ip_arg_len > 50) {
        args.is_parse_successful = false;
        args.parse_error = "invalid ip-address";

        return args;
    }
    else {
        args.target_ip = argv[1];
    }

    size_t port_arg_len = strlen(argv[2]);
    if (port_arg_len == 0 || port_arg_len > 5) {
        args.is_parse_successful = false;
        args.parse_error = "invalid port";

        return args;
    }
    else {
        args.target_port = (unsigned short) strtoul(argv[2], NULL, 0);
    }

    args.is_parse_successful = true;

    return args;
}

void PrintUsage(std::string const& parse_error, char* app_name) {

    std::cout << parse_error
              << std::endl
              << "Usage: " << app_name
              << " <ip-address> <port>"
              << std::endl;
}

int main(int argc, char** argv) {

    spidey::CliArgs args = ParseCliArgs(argc, argv);    

    if (args.is_parse_successful == false) {
        PrintUsage(args.parse_error, argv[0]);
        return 1;
    }

    return spidey::Start(args);
}