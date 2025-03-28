#include "http.hpp"
#include <iostream>

using json = nlohmann::json;

Method resolveMethod(const std::string& method) {
    if (method == "GET") return Method::GET;
    if (method == "POST") return Method::POST;
    if (method == "OPTIONS") return Method::OPTIONS;
    return Method::UNKNOWN;
}

json parse_json_from_request(const std::string& request) {
    size_t body_start = request.find("\r\n\r\n");
    if (body_start != std::string::npos) {
        std::string body = request.substr(body_start + 4); // Bỏ qua header
        try {
            if (!body.empty()) {
                std::cout << "Request body: " << body << std::endl;  // Debug output
                return json::parse(body);  // Parse dữ liệu JSON
            }
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }
    return json();  // Nếu không tìm thấy phần thân, trả về JSON rỗng
}

std::string get_cors_headers() {
    return 
        "Access-Control-Allow-Origin: *\r\n"   // Cho phép tất cả nguồn (origin)
        "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n" // Các phương thức HTTP chấp nhận
        "Access-Control-Allow-Headers: Content-Type, Authorization, ngrok-skip-browser-warning\r\n" // Các header cho phép
        "Access-Control-Max-Age: 86400\r\n"; // Cache preflight response for 24 hours
} 