#include "router.hpp"
#include "response.hpp"
#include "users.hpp"
#include "file_handler.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

Endpoint resolveEndpoint(const std::string& endpoint) {
    if (endpoint == "/auth/login") return Endpoint::AUTH_LOGIN;
    if (endpoint == "/auth/register") return Endpoint::AUTH_REGISTER;
    if (endpoint == "/users/get-all") return Endpoint::USERS_GET_ALL;
    if (endpoint == "/api/test") return Endpoint::FILE_UPLOAD;
    return Endpoint::UNKNOWN;
}

// Helper function to parse multipart form data
std::pair<std::string, std::vector<char>> parseMultipartFormData(const std::string& body, const std::string& boundary) {
    std::string filename;
    std::vector<char> fileData;
    
    // Find the file part in the multipart data
    size_t pos = body.find(boundary);
    if (pos != std::string::npos) {
        // Find the filename in the headers
        size_t filename_pos = body.find("filename=\"", pos);
        if (filename_pos != std::string::npos) {
            size_t filename_end = body.find("\"", filename_pos + 10);
            if (filename_end != std::string::npos) {
                filename = body.substr(filename_pos + 10, filename_end - filename_pos - 10);
            }
        }
        
        // Find the start of file data (after headers)
        size_t data_start = body.find("\r\n\r\n", pos);
        if (data_start != std::string::npos) {
            data_start += 4;
            size_t data_end = body.find(boundary, data_start);
            if (data_end != std::string::npos) {
                // Extract file data
                std::string file_content = body.substr(data_start, data_end - data_start - 2); // -2 for \r\n
                fileData.assign(file_content.begin(), file_content.end());
            }
        }
    }
    
    return {filename, fileData};
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
            switch (endpoint) {
                case Endpoint::FILE_UPLOAD: {
                    try {
                        // Create file handler instance
                        FileHandler fileHandler;
                        
                        // Find the boundary in the Content-Type header
                        std::string boundary;
                        size_t boundary_pos = body.find("boundary=");
                        if (boundary_pos != std::string::npos) {
                            boundary = "--" + body.substr(boundary_pos + 9);
                        }
                        
                        // Parse the multipart form data
                        auto [filename, fileData] = parseMultipartFormData(body, boundary);
                        
                        if (filename.empty() || fileData.empty()) {
                            json response = {
                                {"success", false},
                                {"message", "No file data found"}
                            };
                            sendJsonResponse(socket, response, cors_headers);
                            return;
                        }
                        
                        // Save the file
                        bool success = fileHandler.saveFile(filename, fileData);
                        
                        json response = {
                            {"success", success},
                            {"message", success ? "File uploaded successfully" : "Failed to upload file"},
                            {"filename", filename}
                        };
                        sendJsonResponse(socket, response, cors_headers);
                    } catch (const std::exception& e) {
                        json response = {
                            {"success", false},
                            {"message", std::string("Error processing file: ") + e.what()}
                        };
                        sendJsonResponse(socket, response, cors_headers);
                    }
                    break;
                }
                
                case Endpoint::AUTH_LOGIN: {
                    try {
                        json request_json = json::parse(body);
                        std::string username = request_json["username"];
                        std::string password = request_json["password"];
                        json response = login_response(username, password);
                        sendJsonResponse(socket, response, cors_headers);
                    } catch (const json::exception& e) {
                        std::cerr << "JSON error: " << e.what() << std::endl;
                        sendErrorResponse(socket, "Invalid JSON format", cors_headers);
                    }
                    break;
                }
                
                case Endpoint::AUTH_REGISTER: {
                    try {
                        json request_json = json::parse(body);
                        std::string username = request_json["username"];
                        std::string password = request_json["password"];
                        json response = register_response(username, password);
                        sendJsonResponse(socket, response, cors_headers);
                    } catch (const json::exception& e) {
                        std::cerr << "JSON error: " << e.what() << std::endl;
                        sendErrorResponse(socket, "Invalid JSON format", cors_headers);
                    }
                    break;
                }
                
                default:
                    // Endpoint không được hỗ trợ
                    sendNotFoundResponse(socket, cors_headers);
                    break;
            }
            break;
            
        default:
            // Phương thức không được hỗ trợ
            sendNotFoundResponse(socket, cors_headers);
            break;
    }
} 