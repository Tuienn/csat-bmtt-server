#include "users.hpp"
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <iostream>

bool check_login(const std::string& username, const std::string& password) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    // Mở cơ sở dữ liệu SQLite
    if (sqlite3_open("database.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Truy vấn kiểm tra tài khoản và mật khẩu
    const std::string query = "SELECT * FROM users WHERE username = ? AND password = ? LIMIT 1;";

    // Chuẩn bị câu truy vấn
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Gắn giá trị vào câu truy vấn
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    // Thực thi câu truy vấn
    bool result = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = true; // Nếu có dữ liệu trả về, chứng tỏ người dùng tồn tại
    }

    // Giải phóng bộ nhớ và đóng cơ sở dữ liệu
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return result;
}

nlohmann::json login_response(const std::string& username, const std::string& password) {
    nlohmann::json response;

    if (check_login(username, password)) {
        response["message"] = "Đăng nhập thành công";
        response["isSuccess"] = true;
    } else {
        response["message"] = "Đăng nhập thất bại";
        response["isSuccess"] = false;
    }

    return response;
}

bool user_exists(const std::string& username) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    // Mở cơ sở dữ liệu SQLite
    if (sqlite3_open("database.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Truy vấn kiểm tra tài khoản
    const std::string query = "SELECT id FROM users WHERE username = ? LIMIT 1;";

    // Chuẩn bị câu truy vấn
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Gắn giá trị vào câu truy vấn
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Thực thi câu truy vấn
    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = true; // Người dùng đã tồn tại
    }

    // Giải phóng bộ nhớ và đóng cơ sở dữ liệu
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

int register_user(const std::string& username, const std::string& password) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int user_id = -1;

    // Mở cơ sở dữ liệu SQLite
    if (sqlite3_open("database.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Kiểm tra nếu bảng users chưa tồn tại thì tạo mới
    const std::string create_table = "CREATE TABLE IF NOT EXISTS users ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "username TEXT UNIQUE NOT NULL,"
                                     "password TEXT NOT NULL);";
    
    char* error_msg = nullptr;
    if (sqlite3_exec(db, create_table.c_str(), nullptr, nullptr, &error_msg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << error_msg << std::endl;
        sqlite3_free(error_msg);
        sqlite3_close(db);
        return -1;
    }

    // Truy vấn thêm người dùng mới
    const std::string query = "INSERT INTO users (username, password) VALUES (?, ?);";

    // Chuẩn bị câu truy vấn
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // Gắn giá trị vào câu truy vấn
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    // Thực thi câu truy vấn
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        user_id = sqlite3_last_insert_rowid(db);
    } else {
        std::cerr << "Error inserting new user: " << sqlite3_errmsg(db) << std::endl;
    }

    // Giải phóng bộ nhớ và đóng cơ sở dữ liệu
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return user_id;
}

nlohmann::json register_response(const std::string& username, const std::string& password) {
    nlohmann::json response;

    // Kiểm tra nếu người dùng đã tồn tại
    if (user_exists(username)) {
        response["message"] = "Đăng ký thất bại";
        response["isSuccess"] = false;
    } else {
        // Đăng ký người dùng mới
        int user_id = register_user(username, password);
        
        if (user_id > 0) {
            response["message"] = "Đăng ký thành công";
            response["isSuccess"] = true;
            response["id"] = user_id;
        } else {
            response["message"] = "Đăng ký thất bại";
            response["isSuccess"] = false;
        }
    }

    return response;
}

nlohmann::json get_all_users() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    nlohmann::json response;
    nlohmann::json users = nlohmann::json::array();

    // Mở cơ sở dữ liệu SQLite
    if (sqlite3_open("database.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        response["message"] = "Không thể kết nối đến cơ sở dữ liệu";
        response["isSuccess"] = false;
        return response;
    }

    // Kiểm tra nếu bảng users chưa tồn tại thì tạo mới
    const std::string create_table = "CREATE TABLE IF NOT EXISTS users ("
                                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "username TEXT UNIQUE NOT NULL,"
                                     "password TEXT NOT NULL);";
    
    char* error_msg = nullptr;
    if (sqlite3_exec(db, create_table.c_str(), nullptr, nullptr, &error_msg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << error_msg << std::endl;
        sqlite3_free(error_msg);
        sqlite3_close(db);
        response["message"] = "Lỗi khi kiểm tra bảng dữ liệu";
        response["isSuccess"] = false;
        return response;
    }

    // Truy vấn tất cả người dùng
    const std::string query = "SELECT id, username FROM users;";

    // Chuẩn bị câu truy vấn
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        response["message"] = "Lỗi khi chuẩn bị truy vấn";
        response["isSuccess"] = false;
        return response;
    }

    // Thực thi câu truy vấn và trả về kết quả
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json user;
        
        // Lấy dữ liệu từ kết quả truy vấn
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        
        // Thêm vào đối tượng JSON
        user["id"] = id;
        user["username"] = std::string(reinterpret_cast<const char*>(username));
        
        // Thêm vào mảng users
        users.push_back(user);
    }

    // Giải phóng bộ nhớ và đóng cơ sở dữ liệu
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Tạo phản hồi JSON
    response["message"] = "Lấy danh sách người dùng thành công";
    response["isSuccess"] = true;
    response["users"] = users;

    return response;
}
