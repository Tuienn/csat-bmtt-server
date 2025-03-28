#ifndef USERS_HPP
#define USERS_HPP

#include <string>
#include <nlohmann/json.hpp>

// Kiểm tra đăng nhập của người dùng
bool check_login(const std::string& username, const std::string& password);

// Xử lý đăng nhập trả về kết quả dưới dạng JSON
nlohmann::json login_response(const std::string& username, const std::string& password);

// Kiểm tra nếu người dùng đã tồn tại
bool user_exists(const std::string& username);

// Đăng ký người dùng mới
int register_user(const std::string& username, const std::string& password);

// Xử lý đăng ký trả về kết quả dưới dạng JSON
nlohmann::json register_response(const std::string& username, const std::string& password);

// Lấy danh sách tất cả người dùng
nlohmann::json get_all_users();

#endif // USERS_HPP
