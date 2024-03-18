#include "program.h"
#include <iostream>
#include <filesystem>
#include "SQLiteCpp.h"

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
    SQLite::Database db((fs::current_path() /= DATA_DIR) /= DATA_FILE_NAME,
                        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  db.exec("CREATE TABLE Persons (\n"
            "PersonID int,"
            "LastName varchar(255),"
            "FirstName varchar(255),"
            "Address varchar(255),"
            "City varchar(255));");
}

void Program::init_files() {
    namespace fs = std::filesystem;
    fs::create_directory(fs::current_path() /= DATA_DIR);
    if (!(fs::exists((fs::current_path() /= DATA_DIR) /= DATA_FILE_NAME))) {
        Program::init_db();
    }
}

int Program::start_program(const std::string &version) {
    Program::load_commands();
    Program::init_files();

    std::cout<<"Secure(ish) password manager shell version '"<<version<<"'. Created and maintained by Tiberiu Popescu.\n";
    std::cout<<"Type 'help' to get a list of commands. Type 'quit' to exit the program.\n\n";
    std::string command;
    while(true) {
        std::cout<<">>> ";
        std::cin>>command;
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
    return SC_QUIT;
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


