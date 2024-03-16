#include "program.h"
#include <iostream>
#include <utility>

int Program::start_program(const std::string &version) {
    std::cout<<"Secure(ish) password manager shell version '"<<version<<"'. Created and maintained by Tiberiu Popescu.\n";
    std::cout<<"Type 'help' to get a list of commands. Type 'quit' to exit the program.\n\n";
    std::string command;
    while(true) {
        std::cout<<">>> ";
        std::cin>>command;
        Command cmd(command);
        CmdStatusCode status_code = cmd.execute();
        switch (status_code) {
            case QUIT:
                return 0;
            case UNKNOWN:
                std::cout<<"Unknown command. Type 'help' to get a list of commands.";
                break;
            default:
                break;
        }
    }
}

Command::Command(std::string command) : command(std::move(command)) {}

CmdStatusCode Command::execute() {
    if(command == "quit")
        return QUIT;
    return UNKNOWN;
}
