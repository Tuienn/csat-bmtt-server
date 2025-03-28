#include "database.hpp"
#include <sqlite3.h>
#include <iostream>
#include <nlohmann/json.hpp>


void fetch_data_from_db(nlohmann::json& response) {
    sqlite3* db;
    char* err_msg = 0;
    
    if (sqlite3_open("database.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* query = "SELECT * FROM your_table LIMIT 1;";  // Replace with your actual query.
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        response["id"] = sqlite3_column_int(stmt, 0);
        response["name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
