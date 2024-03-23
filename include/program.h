#ifndef PASSWORD_MANAGER_PROGRAM_H
#define PASSWORD_MANAGER_PROGRAM_H

#include <string>
#include <unordered_map>
#include <memory>
#include "SQLiteCpp.h"
#include "user.h"

constexpr const char* DATA_DIR = "data";
constexpr const char* USERS_DB_FILE_NAME = "users.db3";

class Program {
public:
    static int start_program(const std::string &version);
    static void set_logged_user(User *user);
    static bool user_logged_in();
private:
    static std::unique_ptr<User> logged_user;
    static void init_files();
    static void init_db();
    static void load_commands();
};

#endif //PASSWORD_MANAGER_PROGRAM_H
