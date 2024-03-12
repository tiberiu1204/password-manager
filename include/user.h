#ifndef PASSWORD_MANAGER_USER_H
#define PASSWORD_MANAGER_USER_H
#include "password.h"

class User {
public:
private:
    MasterPassword password;
    std::string username;
};

#endif //PASSWORD_MANAGER_USER_H
