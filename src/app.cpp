#include <string>
#include "app.h"
#include "http/http_client.h"

namespace spidey {

    int Start(CliArgs args) {

        http::HttpClient http_client;

        http_client.HttpGet(args.target_ip, args.target_port);

        return 0;
    }
}