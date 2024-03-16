#ifndef PASSWORD_MANAGER_USER_H
#define PASSWORD_MANAGER_USER_H

#include <string>
#include <cstdint>
#include "user_data.h"

class User {
public:
    User() = default;
    bool validate_password(std::string &password);
    void decrypt_data(const std::string &key);
private:
    uint64_t user_id;
    std::string user_name;
    const std::string password_hash;
    UserData data;
};

#endif //PASSWORD_MANAGER_USER_H
