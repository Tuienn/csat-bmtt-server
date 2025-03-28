#include "server.hpp"
#include "http.hpp"
#include "router.hpp"
#include "response.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;

// Hàm xử lý phiên làm việc cho mỗi kết nối
void session(tcp::socket socket) {
    try {
        boost::asio::streambuf buffer;
        boost::system::error_code error;
        
        // Read the HTTP request headers
        boost::asio::read_until(socket, buffer, "\r\n\r\n", error);
        if (error) {
            std::cerr << "Error reading headers: " << error.message() << std::endl;
            return;
        }

        // Extract the request line and headers
        std::string request_line;
        std::string full_request;
        
        std::istream request_stream(&buffer);
        std::getline(request_stream, request_line);
        request_line.erase(request_line.length() - 1); // Remove \r
        
        std::cout << "Request received: " << request_line << std::endl;  // Debug output
        
        // Read headers and store them
        std::string header;
        while (std::getline(request_stream, header) && header != "\r") {
            full_request += header + "\n";
        }
        
        // Determine the Content-Length if it's a POST request
        size_t content_length = 0;
        if (request_line.find("POST") == 0) {
            std::istringstream header_stream(full_request);
            std::string header_line;
            while (std::getline(header_stream, header_line)) {
                if (header_line.find("Content-Length:") == 0 || header_line.find("content-length:") == 0) {
                    content_length = std::stoi(header_line.substr(header_line.find(":") + 1));
                    break;
                }
            }
        }
        
        // Read the request body if there is content
        std::string body;
        if (content_length > 0) {
            // Calculate how much of the body we've already read
            size_t already_read = buffer.size();
            
            // If we need to read more data
            if (already_read < content_length) {
                std::vector<char> buf(content_length - already_read);
                boost::asio::read(socket, boost::asio::buffer(buf), error);
                if (!error) {
                    body.append(buf.begin(), buf.end());
                }
            }
            
            // If there's still data in the buffer, extract it
            if (!request_stream.eof()) {
                std::string additional_body;
                std::getline(request_stream, additional_body, '\0');
                body = additional_body + body;
            }
            
            std::cout << "Request body: " << body << std::endl;  // Debug output
        }

        // Full request with body
        full_request = request_line + "\r\n" + full_request + "\r\n\r\n" + body;
        
        // Get CORS headers
        std::string cors_headers = get_cors_headers();
        
        // Extract HTTP method and endpoint
        std::string http_method;
        std::string endpoint;
        
        // Parse the request line to extract method and endpoint
        size_t method_end = request_line.find(' ');
        if (method_end != std::string::npos) {
            http_method = request_line.substr(0, method_end);
            
            size_t endpoint_end = request_line.find(' ', method_end + 1);
            if (endpoint_end != std::string::npos) {
                endpoint = request_line.substr(method_end + 1, endpoint_end - method_end - 1);
            }
        }
        
        // Resolve the method and endpoint
        Method method = resolveMethod(http_method);
        Endpoint endp = resolveEndpoint(endpoint);
        
        // Route the request to appropriate handler
        route_request(socket, method, endp, body, cors_headers);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Helper function to resolve HTTP method
Method resolveMethod(const std::string& method) {
    if (method == "GET") return Method::GET;
    if (method == "POST") return Method::POST;
    if (method == "OPTIONS") return Method::OPTIONS;
    return Method::UNKNOWN;
}

// Helper function to resolve endpoint
Endpoint resolveEndpoint(const std::string& endpoint) {
    if (endpoint == "/auth/login") return Endpoint::AUTH_LOGIN;
    if (endpoint == "/auth/register") return Endpoint::AUTH_REGISTER;
    if (endpoint == "/users/get-all") return Endpoint::USERS_GET_ALL;
    return Endpoint::UNKNOWN;
}

// Helper function to send JSON response
void sendJsonResponse(tcp::socket& socket, const json& response, const std::string& cors_headers) {
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

// Helper function to send error response
void sendErrorResponse(tcp::socket& socket, const std::string& error_message, const std::string& cors_headers) {
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

// Helper function to send not found response
void sendNotFoundResponse(tcp::socket& socket, const std::string& cors_headers) {
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

// Hàm khởi tạo server
void start_server() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Server started on port 8080" << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(session, std::move(socket)).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
    }
}
