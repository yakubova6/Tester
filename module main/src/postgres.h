
#include <pqxx/pqxx>
#include <windows.h> // Для SetConsoleOutputCP
#include <vector>
#include <string>
#include <iostream>

std::vector<int> sql_get_array_int(const std::string& column_name, const std::string& table_name, int id);
std::vector<std::string> sql_get_array_str(const std::string& column_name, const std::string& table_name, int id);

void sql_del_from_array_int(const std::string& column_name, const std::string& table_name, int id, int num);
void sql_add_to_array_int(const std::string& column_name, const std::string& table_name, int id, int num);

std::vector<std::string> sql_get_list_str (const std::string& column_name, const std::string& table_name);
std::vector<int> sql_get_list_int (const std::string& column_name, const std::string& table_name);

std::string sql_get_one_str(const std::string& column_name, const std::string& table_name, int id);
int sql_get_one_int(const std::string& column_name, const std::string& table_name, int id);
bool sql_get_one_bool(const std::string& column_name, const std::string& table_name, int id);

void sql_update_one_str(const std::string& column_name, const std::string& table_name, int id, const std::string new_value);
void sql_update_one_bool(const std::string& column_name, const std::string& table_name, int id, bool new_value);

void PostgresInit();
void add_user();

std::string get_db_name();
std::string get_db_password();
