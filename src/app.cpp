#include <string>
#include <iostream>
#include "app.h"
#include "http/http_client.h"
#include "exceptions/appexception.h"

namespace spidey {

    int Start(CliArgs args) {

        http::HttpClient http_client;

        try {
            http_client.HttpGet(args.target_url);
        }
        catch(appex::AppException app_ex) {
            std::cout << "Failure: " << app_ex.what()
                      << std::endl;
        }

        return 0;
    }
}