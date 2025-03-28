#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <boost/asio.hpp>
#include "http.hpp"

// Enumerations for endpoints
enum class Endpoint {
    AUTH_LOGIN,
    AUTH_REGISTER,
    USERS_GET_ALL,
    UNKNOWN
};

// Helper function to resolve endpoint from string
Endpoint resolveEndpoint(const std::string& endpoint);

// Function to handle routing based on method and endpoint
void route_request(
    boost::asio::ip::tcp::socket& socket, 
    Method method, 
    Endpoint endpoint, 
    const std::string& body, 
    const std::string& cors_headers
);

#endif // ROUTER_HPP 