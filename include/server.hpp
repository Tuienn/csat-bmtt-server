#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>
#include "http.hpp"
#include "router.hpp"
#include "response.hpp"

// Xử lý phiên làm việc của mỗi kết nối
void session(boost::asio::ip::tcp::socket socket);

// Xử lý yêu cầu của server
void start_server();

#endif // SERVER_HPP
