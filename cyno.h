#pragma once

#include <crow.h>
#include <openssl/sha.h>  // For SHA-1 hashing
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>

class CynoHandler {
public:
    static void handle_request(const crow::request& req, crow::response& res) {
        // Extract necessary request data
        std::string host = req.get_header_value("Host");
        std::string uri = req.url;
        std::string ip = req.remote_ip_address;
        std::string ua = req.get_header_value("User-Agent");

        // Set headers with SHA-1 encoded values
        set_headers(res, uri, ip, ua, host);

        bool locked = false;  // Customize this logic as needed
        auto current_time = std::chrono::system_clock::now();

        if (req.url != "/404" && locked) {
            send_locked_response(res, current_time);
        } else {
            res.write("Request processed successfully");
            res.end();
        }
    }

private:
    static std::string generate_sha1(const std::string& data) {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);

        std::ostringstream ss;
        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    static void set_headers(crow::response& res, const std::string& uri, const std::string& ip,
                            const std::string& ua, const std::string& host) {
        auto current_time = std::chrono::system_clock::now();
        auto time_in_micro = std::chrono::duration_cast<std::chrono::microseconds>(current_time.time_since_epoch()).count();

        // Generate Cyno-RequestID
        std::string request_id_data = uri + ip + ua + std::to_string(time_in_micro);
        res.set_header("Cyno-RequestID", generate_sha1(request_id_data));

        // Generate Cyno-ClientID
        std::string client_id_data = host + ip + ua;
        res.set_header("Cyno-ClientID", generate_sha1(client_id_data));
    }

    static void send_locked_response(crow::response& res, std::chrono::system_clock::time_point current_time) {
        res = crow::response(403);
        res.set_header("Content-Type", "application/json");

        std::string time_str = std::to_string(current_time.time_since_epoch().count());
        std::string request_id = res.get_header_value("Cyno-RequestID");
        std::string client_id = res.get_header_value("Cyno-ClientID");

        // Custom JSON response
        res.write("{\"code\":-1,\"stable\":false,"
                  "\"message\":\"大风机关赛诺盯上你了(您的网络环境存在安全风险 我们无法提供服务)[Errno -1]\","
                  "\"time\":\"" + time_str + "\","
                  "\"requestID\":\"" + request_id + "\","
                  "\"clientID\":\"" + client_id + "\"}");
        res.end();
    }
};

