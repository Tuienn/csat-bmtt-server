#include "response.hpp"
#include <iostream>

using json = nlohmann::json;

void sendJsonResponse(boost::asio::ip::tcp::socket& socket, const json& response, const std::string& cors_headers) {
    std::string json_response = response.dump();
    std::string response_message = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(json_response.length()) + "\r\n" +
        cors_headers +
        "\r\n" + 
        json_response;

    boost::asio::write(socket, boost::asio::buffer(response_message));
}

void sendErrorResponse(boost::asio::ip::tcp::socket& socket, const std::string& error_message, const std::string& cors_headers) {
    std::string error_json = "{\"error\":\"" + error_message + "\"}";
    std::string response = 
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(error_json.length()) + "\r\n" +
        cors_headers +
        "\r\n" + 
        error_json;
    boost::asio::write(socket, boost::asio::buffer(response));
}

void sendNotFoundResponse(boost::asio::ip::tcp::socket& socket, const std::string& cors_headers) {
    std::string not_found = "Not Found";
    std::string response = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(not_found.length()) + "\r\n" +
        cors_headers + 
        "\r\n" +
        not_found;
    boost::asio::write(socket, boost::asio::buffer(response));
}

void sendOptionsResponse(boost::asio::ip::tcp::socket& socket, const std::string& cors_headers) {
    std::string response = 
        "HTTP/1.1 204 No Content\r\n" +
        cors_headers +
        "Content-Length: 0\r\n\r\n";
    boost::asio::write(socket, boost::asio::buffer(response));
} 