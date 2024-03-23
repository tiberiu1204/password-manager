#ifndef PASSWORD_MANAGER_USER_H
#define PASSWORD_MANAGER_USER_H

#include <string>
#include <cstdint>
#include "user_data.h"

class User {
public:
    User(uint64_t user_id, std::string username, std::string date_created);
    ~User();
    User &operator=(const User &other);
    User(const User &other);
    void retrieve_data();
private:
    // TODO: make const
    uint64_t user_id;
    std::string username;
    std::string date_created;
    UserData data;
};

#endif //PASSWORD_MANAGER_USER_H
