#include "program.h"
#include <iostream>

void Command::add_cmd(const std::string &cmd_name, Command::Cmd cmd_code) {
    Command::commands[cmd_name] = cmd_code;
}

void Program::load_commands() {
    Command::add_cmd("quit", Command::QUIT);
    Command::add_cmd("add user", Command::ADD_USER);
    Command::add_cmd("delete user", Command::DELETE_USER);
    Command::add_cmd("login", Command::LOGIN);
}

int Program::start_program(const std::string &version) {
    Program::load_commands();

    std::cout<<"Secure(ish) password manager shell version '"<<version<<"'. Created and maintained by Tiberiu Popescu.\n";
    std::cout<<"Type 'help' to get a list of commands. Type 'quit' to exit the program.\n\n";
    std::string command;
    while(true) {
        std::cout<<">>> ";
        std::cin>>command;
        Command cmd(command);
        CmdStatusCode status_code = cmd.execute();
        switch (status_code) {
            case SC_QUIT:
                return 0;
            case SC_UNKNOWN:
                std::cout<<"Unknown command. Type 'help' to get a list of commands.";
                break;
            default:
                break;
        }
    }
}

Command::Command(const std::string &command) {
    if(Command::commands.find(command) != Command::commands.end()) {
        this->command = Command::commands[command];
    }
    else {
        this->command = UNKNOWN;
    }
}

bool Command::validate_username(const std::string &username) {
    return username.size() <= 32 && username.size() >= 6;
}

bool Command::validate_password(const std::string &password) {
    return password.size() >= 6 && password.size() <= 72;
}

bool Command::add_user() {
    std::string username;
    std::string password;
    std::cout<<"Username: ";
    std::cin>>username;
    if(!Command::validate_username(username)) {
        return false;
    }
    std::cout<<"\n";
    std::cout<<"Password: ";
    std::cin>>password;
    if(!Command::validate_password(password)) {
        return false;
    }

    return true;
}

CmdStatusCode Command::execute() {
    bool success = false;
    switch (this->command) {
        case QUIT:
            return SC_QUIT;
        case ADD_USER:
            try {
                success = Command::add_user();
            }
            catch (const std::exception &e) {
                return SC_ERROR;
            }
        default:
            return SC_UNKNOWN;
    }
    if(success) return SC_SUCCESS;
    return SC_FAIL;
}
