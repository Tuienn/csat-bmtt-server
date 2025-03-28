#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

// Helper function to send JSON response
void sendJsonResponse(boost::asio::ip::tcp::socket& socket, const nlohmann::json& response, const std::string& cors_headers);

// Helper function to send error response
void sendErrorResponse(boost::asio::ip::tcp::socket& socket, const std::string& error_message, const std::string& cors_headers);

// Helper function to send not found response
void sendNotFoundResponse(boost::asio::ip::tcp::socket& socket, const std::string& cors_headers);

// Helper function to send OPTIONS response for CORS
void sendOptionsResponse(boost::asio::ip::tcp::socket& socket, const std::string& cors_headers);

#endif // RESPONSE_HPP 