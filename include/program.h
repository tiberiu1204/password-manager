#ifndef PASSWORD_MANAGER_PROGRAM_H
#define PASSWORD_MANAGER_PROGRAM_H

#include <string>

enum CmdStatusCode {
    SUCCESS,
    FAIL,
    UNKNOWN,
    QUIT
};

class Program {
public:
    static int start_program(const std::string &verison);
};

class Command {
public:
    Command(std::string command);
    CmdStatusCode execute();
private:
    std::string command;
};

#endif //PASSWORD_MANAGER_PROGRAM_H
