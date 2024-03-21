#ifndef PASSWORD_MANAGER_PROGRAM_H
#define PASSWORD_MANAGER_PROGRAM_H

#include <string>
#include <unordered_map>
#include <memory>
#include "SQLiteCpp.h"

constexpr const char* DATA_DIR = "data";
constexpr const char* USERS_DB_FILE_NAME = "users.db3";

class Program {
private:
    static void load_commands();
public:
    static int start_program(const std::string &version);
    static void init_files();
    static void init_db();
};

#endif //PASSWORD_MANAGER_PROGRAM_H
