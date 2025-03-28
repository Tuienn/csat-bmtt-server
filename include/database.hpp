#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <nlohmann/json.hpp>

void fetch_data_from_db(nlohmann::json& response);

#endif // DATABASE_HPP
