#include "server.hpp"
#include "users.hpp"
#include <iostream>
#include <sstream>
#include <map>

// Server constructor implementation
Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket_(io_context) {
    do_accept();
}

// Helper function to parse the body of the POST request (URL-encoded form data)
std::map<std::string, std::string> parse_form_data(const std::string& body) {
    std::map<std::string, std::string> form_data;
    std::stringstream ss(body);
    std::string item;
    while (std::getline(ss, item, '&')) {
        size_t pos = item.find('=');
        if (pos != std::string::npos) {
            std::string key = item.substr(0, pos);
            std::string value = item.substr(pos + 1);
            form_data[key] = value;
        }
    }
    return form_data;
}

// Function to handle POST requests for user login
void handle_login_request(const std::string& request, std::string& response) {
    // Extract the body from the POST request
    size_t body_pos = request.find("\r\n\r\n");
    if (body_pos != std::string::npos) {
        std::string body = request.substr(body_pos + 4);
        auto form_data = parse_form_data(body);

        std::string username = form_data["username"];
        std::string password = form_data["password"];

        // Check if the user exists in the database
        bool login_success = check_user_credentials(username, password);

        if (login_success) {
            response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"
                       "{\"message\": \"Đăng nhập thành công\", \"isSuccess\": true}";
        } else {
            response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n"
                       "{\"message\": \"Đăng nhập thất bại\", \"isSuccess\": false}";
        }
    } else {
        response = "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n"
                   "{\"message\": \"Invalid request body\", \"isSuccess\": false}";
    }
}

void Server::do_accept() {
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec) {
            // Simple request handling
            char buffer[1024];
            size_t len = socket_.read_some(boost::asio::buffer(buffer));
            std::string request(buffer, len);

            std::string response;
            if (request.find("POST /auth/login") != std::string::npos) {
                handle_login_request(request, response);
            } else {
                response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
            }

            boost::asio::write(socket_, boost::asio::buffer(response));
            socket_.close();
        }
        do_accept(); // Continue accepting new connections
    });
}