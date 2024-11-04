# cyno.h - Yet another firewall middleware for Crow

## Features:
1. Automatic Ban CC Attack
2. Record User Footstep by Clientid
3. etc

## How to use:
```c++
#include "cyno.h"

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](const crow::request& req, crow::response& res) {
        CynoHandler::handle_request(req, res);
    });

    app.port(8080).multithreaded().run();
}
```
## License:

Under GPL-3.0 License.

## Source:

https://github.com/chi-net/cyno
