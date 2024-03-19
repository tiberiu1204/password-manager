#include "program.h"
#include <iostream>
#include <filesystem>
#include "SQLiteCpp.h"
#include <conio.h>


constexpr const char* INIT_DB_QUERY = "create table users "
                                      "(user_id int not null primary key,"
                                      "username varchar(64),"
                                      "password_hash varchar(64),"
                                      "date_created date);";

std::unordered_map<std::string, std::unique_ptr<Command>> Command::commands;

void Command::add_cmd(const std::string &cmd_name, std::unique_ptr<Command> cmd) {
    Command::commands[cmd_name] = std::move(cmd);
}

Command *Command::get_cmd(const std::string &cmd_name) {
    if(Command::commands.find(cmd_name) == Command::commands.end()) return nullptr;
    return Command::commands[cmd_name].get();
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
    db.exec(INIT_DB_QUERY);
}

void Program::init_files() {
    namespace fs = std::filesystem;
    fs::create_directory(fs::current_path() /= DATA_DIR);
    if (!(fs::exists((fs::current_path() /= DATA_DIR) /= USERS_DB_FILE_NAME))) {
        Program::init_db();
    }
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
            std::cout<<"Unknown command. Type 'help' to get a list of commands.\n";
            continue;
        }
        CmdStatusCode status_code = cmd->execute();
        switch (status_code) {
            case SC_QUIT:
                return 0;
            default:
                break;
        }
    }
}

CmdStatusCode CmdQuit::execute() {
    return SC_QUIT;
}

void CmdQuit::display_help() {
    std::cout<<"quit\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Exits the program.\n\n";
}

CmdStatusCode CmdAddUser::execute() {
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
    return SC_SUCCESS;
}

bool CmdAddUser::validate_username(const std::string& username) {
    bool valid = true;
    if(username.size() >= 64) {
        std::cout<<"\n[ERROR] Username can be at most 63 characters long!\n";
        valid = false;
    }
    return valid;
}

bool CmdAddUser::validate_password(const std::string& password) {
    bool valid = true;
    if(password.size() < 8) {
        std::cout<<"\n[ERROR] Password must be at least 8 characters long!\n";
        valid = false;
    }
    if(password.find(' ') != std::string::npos) {
        if(valid) std::cout<<"\n";
        std::cout<<"[ERROR] Password must not contain spaces!\n";
        valid = false;
    }
    return valid;
}

void CmdAddUser::display_help() {
    std::cout<<"add user\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and a password for a new user.\n\n";
}

CmdStatusCode CmdDeleteUser::execute() {
    return SC_QUIT;
}

void CmdDeleteUser::display_help() {
    std::cout<<"delete user\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and it's corresponding password in order to delete the user"
               "and all its stored information.\n\n";
}

CmdStatusCode CmdHelp::execute() {
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

CmdStatusCode CmdLogin::execute() {
    return SC_QUIT;
}

void CmdLogin::display_help() {
    std::cout<<"login\n";
    std::cout<<"DESCRIPTION:\n";
    std::cout<<"Prompts the user to insert a username and a password in order to login into the account.\n\n";
}


