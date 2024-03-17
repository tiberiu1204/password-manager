#ifndef PASSWORD_MANAGER_PROGRAM_H
#define PASSWORD_MANAGER_PROGRAM_H

#include <string>
#include <unordered_map>
#include <memory>

enum CmdStatusCode {
    SC_SUCCESS,
    SC_FAIL,
    SC_ERROR,
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
    virtual CmdStatusCode execute() = 0;
    virtual void display_help() = 0;
    static void add_cmd(const std::string &cmd_name, std::unique_ptr<Command> cmd);
    static Command *get_cmd(const std::string &cmd_name);
    virtual ~Command() = default;

protected:
    static std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};

class CmdQuit : public Command {
public:
    CmdStatusCode execute() override;
    void display_help() override;
};
class CmdAddUser : public Command {
public:
    CmdStatusCode execute() override;
    void display_help() override;
};
class CmdDeleteUser : public Command {
public:
    CmdStatusCode execute() override;
    void display_help() override;
};
class CmdLogin : public Command {
public:
    CmdStatusCode execute() override;
    void display_help() override;
};
class CmdHelp : public Command {
public:
    CmdStatusCode execute() override;
    void display_help() override;
};



#endif //PASSWORD_MANAGER_PROGRAM_H
