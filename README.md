# cyno.h - Yet another firewall middleware for Crow

## Features:
1. Automatic Ban CC Attack
2. Record User Footstep by Clientid
3. etc

## How to use:
```c++
#include "cyno.h"

int main() {
    // Initialize the Crow application
    crow::App<crow::Cyno> app;

    // Define the /ping route
    CROW_ROUTE(app, "/ping")
    ([](const crow::request&, crow::response& res) {
        crow::json::wvalue response_json;
        response_json["status"] = 200;
        response_json["ping"] = "pong";
        
        res.write(response_json.dump());
        res.end();
    });

    // Run the application on port 8080
    app.port(8080).multithreaded().run();
}
```
## License:

Under GPL-3.0 License.

## Source:

https://github.com/chi-net/cyno
