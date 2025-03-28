#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <nlohmann/json.hpp>

// Enumerations for HTTP methods
enum class Method {
    GET,
    POST,
    OPTIONS,
    UNKNOWN
};

// Helper function to resolve HTTP method from string
Method resolveMethod(const std::string& method);

// Parse JSON from HTTP request
nlohmann::json parse_json_from_request(const std::string& request);

// Get CORS headers for responses
std::string get_cors_headers();

#endif // HTTP_HPP 