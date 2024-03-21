#include "commands.h"
#include <iostream>
#include <conio.h>
#include "SQLiteCpp.h"
#include <filesystem>
#include <sstream>
#include <ctime>
#include <chrono>
#include "bcrypt.h"

extern constexpr const char* DATA_DIR = "data";
extern constexpr const char* USERS_DB_FILE_NAME = "users.db3";
constexpr const uint8_t BCRYPT_COST = 12;

std::unordered_map<std::string, std::unique_ptr<Command>> Command::commands;

Command::CmdStatusCode CmdQuit::execute() {
    return SC_QUIT;
}

void CmdQuit::display_help() {
    std::cout<<"quit\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Exits the program.\n\n";
}

Command::CmdStatusCode CmdAddUser::execute() {
    std::cout<<"\nYou will be prompted to insert a username and a password for your new account. "
               "Type '!abort' if you want to cancel the process at any time.\n\n";
    std::string username;
    while(true) {
        std::cout<<"Username: ";
        std::getline(std::cin, username);
        if(username == "!abort") {
            std::cout<<"\n";
            return SC_FAIL;
        }
        if(CmdAddUser::validate_username(username)) {
            break;
        }
    }
    std::string password;
    while(true) {
        std::cout<<"Password: ";
        char input_ch;
        while(true) {
            input_ch = static_cast<char>(getch());
            if(input_ch == 8) {
                if(!password.empty()) password.pop_back();
            }
            else if(input_ch < 32) {
                break;
            }
            else {
                password.push_back(input_ch);
            }
        }
        if(password == "!abort") {
            std::cout<<"\n";
            return SC_FAIL;
        }
        if(CmdAddUser::validate_password(password)) {
            break;
        }
        password.clear();
    }
    std::string password_hash = Bcrypt().hash(password, Bcrypt::generate_salt(), BCRYPT_COST);
    CmdAddUser::insert_user(username, password_hash);
    return SC_SUCCESS;
}

bool CmdAddUser::validate_username(const std::string& username) {
    bool valid = true;
    if(username.size() >= 64) {
        std::cout<<"[ERROR] Username can be at most 63 characters long!\n";
        valid = false;
    }
    return valid;
}

bool CmdAddUser::validate_password(const std::string& password) {
    bool valid = true;
    if(password.size() < 8) {
        std::cout<<"[ERROR] Password must be at least 8 characters long!\n";
        valid = false;
    }
    if(password.find(' ') != std::string::npos) {
        std::cout<<"[ERROR] Password must not contain spaces!\n";
        valid = false;
    }
    return valid;
}

void CmdAddUser::insert_user(const std::string& username, const std::string& password_hash) {
    std::stringstream s;
    auto current_time_point = std::chrono::system_clock::now();
    auto current_time = std::chrono::system_clock::to_time_t(current_time_point);
    s<<"insert into users (username, password_hash, date_created)"
       "values ('"<<username<<"', '"<<password_hash<<"', '"<<
       std::put_time(std::localtime(&current_time), "%Y-%m-%d")<<"');";
    SQLite::Database db((std::filesystem::current_path() /= DATA_DIR) /= USERS_DB_FILE_NAME,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    db.exec(s.str());
}

void CmdAddUser::display_help() {
    std::cout<<"add user\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and a password for a new user.\n\n";
}

Command::CmdStatusCode CmdDeleteUser::execute() {
    return SC_QUIT;
}

void CmdDeleteUser::display_help() {
    std::cout<<"delete user\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and it's corresponding password in order to delete the user"
               "and all its stored information.\n\n";
}

Command::CmdStatusCode CmdHelp::execute() {
    std::cout<<"Displaying list of commands.\n\n";
    for(const auto &cmd_pair : Command::commands) {
        cmd_pair.second->display_help();
    }
    return SC_SUCCESS;
}

void CmdHelp::display_help() {
    std::cout<<"help\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Displays a complete list of commands and their descriptions.\n\n";
}

Command::CmdStatusCode CmdLogin::execute() {
    return SC_QUIT;
}

void CmdLogin::display_help() {
    std::cout<<"login\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and a password in order to login into the account.\n\n";
}

void Command::add_cmd(const std::string &cmd_name, std::unique_ptr<Command> cmd) {
    Command::commands[cmd_name] = std::move(cmd);
}

Command *Command::get_cmd(const std::string &cmd_name) {
    if(Command::commands.find(cmd_name) == Command::commands.end()) return nullptr;
    return Command::commands[cmd_name].get();
}