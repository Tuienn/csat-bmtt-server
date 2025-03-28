#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

// Enumerations for HTTP methods and endpoints
enum class Method {
    GET,
    POST,
    OPTIONS,
    UNKNOWN
};

enum class Endpoint {
    AUTH_LOGIN,
    AUTH_REGISTER,
    USERS_GET_ALL,
    UNKNOWN
};

// Helper function to resolve HTTP method
Method resolveMethod(const std::string& method);

// Helper function to resolve endpoint
Endpoint resolveEndpoint(const std::string& endpoint);

// Helper function to send JSON response
void sendJsonResponse(boost::asio::ip::tcp::socket& socket, const nlohmann::json& response, const std::string& cors_headers);

// Helper function to send error response
void sendErrorResponse(boost::asio::ip::tcp::socket& socket, const std::string& error_message, const std::string& cors_headers);

// Helper function to send not found response
void sendNotFoundResponse(boost::asio::ip::tcp::socket& socket, const std::string& cors_headers);

// Xử lý phiên làm việc của mỗi kết nối
void session(boost::asio::ip::tcp::socket socket);

// Xử lý yêu cầu của server
void start_server();

#endif // SERVER_HPP
