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
using json = nlohmann::json;

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
