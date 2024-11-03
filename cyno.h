#ifndef CYNO_H
#define CYNO_H

#include <crow.h>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

class Cyno {
public:
    static crow::middleware::context Default() {
        return crow::middleware::context([](crow::request& req, crow::response& res, crow::context& /* ctx */) {
            // Generate request ID based on URI, IP, User-Agent, and timestamp
            std::string uri = req.url;
            std::string ip = req.remote_endpoint().address().to_string();
            std::string ua = req.get_header_value("User-Agent");
            auto now = std::chrono::system_clock::now();
            auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

            // Compute SHA1 hash for RequestID
            std::string request_id = computeSHA1(uri + ip + ua + std::to_string(time_us));
            res.add_header("Cyno-RequestID", request_id);

            // Compute SHA1 hash for ClientID
            std::string host = req.get_header_value("Host");
            std::string client_id = computeSHA1(host + ip + ua);
            res.add_header("Cyno-ClientID", client_id);

            // Check if request is locked
            bool locked = false;
            auto current_time = std::chrono::system_clock::to_time_t(now);

            if (req.url != "/404" && locked) {
                res.code = crow::status::FORBIDDEN;
                res.write(crow::json::wvalue{
                    {"code", -1},
                    {"stable", false},
                    {"message", "大风机关赛诺盯上你了(您的网络环境存在安全风险 我们无法提供服务)[Errno -1]"},
                    {"time", std::put_time(std::gmtime(&current_time), "%Y-%m-%d %H:%M:%S")},
                    {"requestID", request_id},
                    {"clientID", client_id}
                }.dump());
                res.end();
                return;
            }
            res.next();
        });
    }

private:
    static std::string computeSHA1(const std::string& input) {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
        std::ostringstream os;
        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
            os << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        return os.str();
    }
};

#endif // CYNO_H

