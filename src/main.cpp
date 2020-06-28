#include <iostream>
#include <regex>
#include <exception>
#include <string.h>
#include <cstdlib>
#include <exception>
#include "app.h"

spidey::CliArgs ParseCliArgs(int argc, char** argv) {

    spidey::CliArgs args;

    if (argc != 2) {
        args.is_parse_successful = false;
        args.parse_error = "missing arguments";
        
        return args;
    }

    std::string host_url = argv[1];
    std::regex url_regex("^((http[s]?):\\/\\/)?([a-zA-Z0-9-.]+)(:(\\d+))?(\\/(.*))?$");

    if (std::regex_search(host_url, url_regex)) {
        args.target_url = host_url;
        args.is_parse_successful = true;
    }
    else {
        args.is_parse_successful = false;
        args.parse_error = "invalid host URL";
    }

    return args;
}

void PrintUsage(std::string const& parse_error, char* app_name) {

    std::cout << parse_error
              << std::endl
              << "Usage: " << app_name
              << " <url>"
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