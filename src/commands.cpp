#include "commands.h"
#include <iostream>
#include <conio.h>
#include "SQLiteCpp.h"
#include <filesystem>
#include <sstream>
#include <ctime>
#include <chrono>
#include "bcrypt.h"

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
    if(username.size() < 6) {
        valid = false;
        std::cout<<"[ERROR] Username must be at least 6 characters long.\n";
    }
    if(username.find(' ') != std::string::npos) {
        std::cout<<"[ERROR] Username must not contain spaces!\n";
        valid = false;
    }
    SQLite::Database db((std::filesystem::current_path() /= DATA_DIR) /= USERS_DB_FILE_NAME,
                        SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    try {
        if(db.execAndGet("select username from users where username = '" + username + "'").size() != 0) {
            valid = false;
            std::cout<<"[ERROR] Username already exists.\n";
        }
    }
    catch(std::exception &e) {}
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
    if(Program::user_logged_in()) {
        std::cout<<"[ERROR] Another user already logged in. You must logout before logging into another user.\n\n";
        return SC_FAIL;
    }
    std::cout<<"\nYou will be prompted to insert a username and a password for your account. "
               "Type '!abort' if you want to cancel the process at any time.\n\n";
    SQLite::Database db((std::filesystem::current_path() /= DATA_DIR) /= USERS_DB_FILE_NAME);
    while(true) {
        std::string username;
        std::string password;

        std::cout << "Username: ";
        std::getline(std::cin, username);
        if (username == "!abort") {
            std::cout << "\n";
            return SC_FAIL;
        }

        std::cout << "Password: ";
        char input_ch;
        while (true) {
            input_ch = static_cast<char>(getch());
            if (input_ch == 8) {
                if (!password.empty()) password.pop_back();
            } else if (input_ch < 32) {
                break;
            } else {
                password.push_back(input_ch);
            }
        }
        if (password == "!abort") {
            std::cout << "\n";
            return SC_FAIL;
        }


        const std::string query_body = "select user_id, username, password_hash, date_created from users "
                                  "where username = '" + username + "';";
        SQLite::Statement query(db, query_body);
        try {
            query.executeStep();
        }
        catch(std::exception &e) {
            std::cout<<"\n[ERROR] Wrong username or password. Try again.\n\n";
            continue;
        }

        uint64_t q_user_id = static_cast<uint64_t>(query.getColumn(0).getInt64());
        std::string q_username = query.getColumn(1);
        std::string q_password_hash = query.getColumn(2);
        std::string q_date_created = query.getColumn(3);

        std::string password_hash = Bcrypt().hash(password, Bcrypt::extract_salt(q_password_hash),
                                                  Bcrypt::extract_cost(q_password_hash));

        if(q_username == username && q_password_hash == password_hash) {
            User *user = new User(q_user_id, username, q_date_created);
            user->retrieve_data();
            Program::set_logged_user(user);
            return SC_SUCCESS;
        }
        else {
            std::cout<<"\n[ERROR] Wrong username or password. Try again.\n\n";
        }
    }
}

void CmdLogin::display_help() {
    std::cout<<"login\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and a password in order to login into the account. "
               "You must logout before you can login to another user.\n\n";
}

void Command::add_cmd(const std::string &cmd_name, std::unique_ptr<Command> cmd) {
    Command::commands[cmd_name] = std::move(cmd);
}

Command *Command::get_cmd(const std::string &cmd_name) {
    if(Command::commands.find(cmd_name) == Command::commands.end()) return nullptr;
    return Command::commands[cmd_name].get();
}