#ifndef PASSWORD_MANAGER_PROGRAM_H
#define PASSWORD_MANAGER_PROGRAM_H

#include <string>
#include <unordered_map>

enum CmdStatusCode {
    SC_SUCCESS,
    SC_FAIL,
    SC_ERROR,
    SC_UNKNOWN,
    SC_QUIT
};

class Program {
private:
    static void load_commands();
public:
    static int start_program(const std::string &version);
};

class Command {
public:
    explicit Command(const std::string &command);
    CmdStatusCode execute();
    enum Cmd {
        QUIT,
        ADD_USER,
        DELETE_USER,
        LOGIN,
        HELP,
        UNKNOWN
    };
    static void add_cmd(const std::string &cmd_name, Cmd cmd_code);
private:
    Cmd command;
    static std::unordered_map<std::string, Cmd> commands;
    static bool add_user();
    static bool validate_username(const std::string &);
    static bool validate_password(const std::string &);
public:
};

#endif //PASSWORD_MANAGER_PROGRAM_H
