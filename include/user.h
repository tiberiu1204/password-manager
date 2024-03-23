#ifndef PASSWORD_MANAGER_USER_H
#define PASSWORD_MANAGER_USER_H

#include <string>
#include <cstdint>
#include "user_data.h"

class User {
public:
    User(uint64_t user_id, std::string username);
    void retrieve_data();
private:
    const uint64_t user_id;
    std::string username;
    UserData data;
};

#endif //PASSWORD_MANAGER_USER_H
