#ifndef PASSWORD_MANAGER_COMMANDS_H
#define PASSWORD_MANAGER_COMMANDS_H

#include <memory>
#include <string>
#include <unordered_map>

class Command {
public:
    enum CmdStatusCode {
        SC_SUCCESS,
        SC_FAIL,
        SC_ERROR,
        SC_QUIT
    };
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
private:
    static bool validate_username(const std::string&);
    static bool validate_password(const std::string&);
    static void insert_user(const std::string&, const std::string&);
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

#endif //PASSWORD_MANAGER_COMMANDS_H
