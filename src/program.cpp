#include "program.h"
#include <iostream>
#include <filesystem>
#include "SQLiteCpp.h"
#include "commands.h"

constexpr const char* INIT_USERS_TABLE = "create table if not exists users "
                                      "("
                                      "user_id integer not null primary key autoincrement,"
                                      "username varchar(64),"
                                      "password_hash varchar(64),"
                                      "date_created date"
                                      ");";

constexpr const char* INIT_USER_DATA_TABLE = "create table if not exists user_data "
                                             "("
                                             "user_id integer not null,"
                                             "app_name varchar(128),"
                                             "website_link varchar(256),"
                                             "username varchar(128),"
                                             "password varchar(128),"
                                             "notes varchar(2048),"
                                             "primary key (user_id autoincrement),"
                                             "foreign key (user_id) references users(user_id) on delete cascade"
                                             ");";


std::unique_ptr<User> Program::logged_user = nullptr;

void Program::set_logged_user(User *user) {
    Program::logged_user.reset(user);
}

bool Program::user_logged_in() {
    return !(Program::logged_user == nullptr);
}

void Program::load_commands() {
    Command::add_cmd("quit", std::make_unique<CmdQuit>());
    Command::add_cmd("add user", std::make_unique<CmdAddUser>());
    Command::add_cmd("delete user", std::make_unique<CmdDeleteUser>());
    Command::add_cmd("login", std::make_unique<CmdLogin>());
    Command::add_cmd("help", std::make_unique<CmdHelp>());
}

void Program::init_db() {
    namespace fs = std::filesystem;
    SQLite::Database db((fs::current_path() /= DATA_DIR) /= USERS_DB_FILE_NAME,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec("PRAGMA foreign_keys = ON;");
    db.exec(INIT_USERS_TABLE);
    db.exec(INIT_USER_DATA_TABLE);
}

void Program::init_files() {
    namespace fs = std::filesystem;
    Program::init_db();
}

int Program::start_program(const std::string &version) {
    Program::load_commands();
    Program::init_files();

    std::cout<<"Secure(ish) password manager shell version '"<<version<<"'. Created and maintained by Tiberiu Popescu.\n";
    std::cout<<"Type 'help' to get a list of commands. Type 'quit' to exit the program.\n";
    std::string command;
    while(true) {
        std::cout<<"\n>>> ";
        std::getline(std::cin, command);
        Command *cmd = Command::get_cmd(command);
        if(!cmd) {
            std::cout<<"\nUnknown command. Type 'help' to get a list of commands.\n";
            continue;
        }
        Command::CmdStatusCode status_code = cmd->execute();
        switch (status_code) {
            case Command::SC_QUIT:
                return 0;
            case Command::SC_SUCCESS:
                std::cout<<"Command executed successfully.\n";
                break;
            case Command::SC_FAIL:
                std::cout<<"Command failed.\n";
                break;
            case Command::SC_ERROR:
                std::cout<<"An internal error has occurred.\n";
                break;
            default:
                break;
        }
    }
}
