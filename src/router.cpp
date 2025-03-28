#include "router.hpp"
#include "response.hpp"
#include "users.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Endpoint resolveEndpoint(const std::string& endpoint) {
    if (endpoint == "/auth/login") return Endpoint::AUTH_LOGIN;
    if (endpoint == "/auth/register") return Endpoint::AUTH_REGISTER;
    if (endpoint == "/users/get-all") return Endpoint::USERS_GET_ALL;
    return Endpoint::UNKNOWN;
}

void route_request(
    boost::asio::ip::tcp::socket& socket, 
    Method method, 
    Endpoint endpoint, 
    const std::string& body, 
    const std::string& cors_headers
) {
    // Handle different request methods
    switch (method) {
        case Method::OPTIONS:
            // Handle OPTIONS request for CORS preflight
            sendOptionsResponse(socket, cors_headers);
            break;
            
        case Method::GET:
            // Handle GET endpoints
            switch (endpoint) {
                case Endpoint::USERS_GET_ALL: {
                    // Lấy danh sách tất cả người dùng
                    json response = get_all_users();
                    sendJsonResponse(socket, response, cors_headers);
                    break;
                }
                
                default:
                    // Endpoint không được hỗ trợ
                    sendNotFoundResponse(socket, cors_headers);
                    break;
            }
            break;
            
        case Method::POST:
            // Handle POST endpoints
            try {
                json request_json = json::parse(body);
                
                switch (endpoint) {
                    case Endpoint::AUTH_LOGIN: {
                        // Đăng nhập
                        std::string username = request_json["username"];
                        std::string password = request_json["password"];
                        json response = login_response(username, password);
                        sendJsonResponse(socket, response, cors_headers);
                        break;
                    }
                    
                    case Endpoint::AUTH_REGISTER: {
                        // Đăng ký
                        std::string username = request_json["username"];
                        std::string password = request_json["password"];
                        json response = register_response(username, password);
                        sendJsonResponse(socket, response, cors_headers);
                        break;
                    }
                    
                    default:
                        // Endpoint không được hỗ trợ
                        sendNotFoundResponse(socket, cors_headers);
                        break;
                }
            } catch (const json::exception& e) {
                std::cerr << "JSON error: " << e.what() << std::endl;
                sendErrorResponse(socket, "Invalid JSON format", cors_headers);
            }
            break;
            
        default:
            // Phương thức không được hỗ trợ
            sendNotFoundResponse(socket, cors_headers);
            break;
    }
} 