#ifndef PASSWORD_MANAGER_PASSWORD_H
#define PASSWORD_MANAGER_PASSWORD_H

#include <string>

class MasterPassword {
private:
    std::string password_hash;
    std::string bcrypt_hash(const std::string&);

public:
    MasterPassword();
    MasterPassword(const std::string&);
};

#endif //PASSWORD_MANAGER_PASSWORD_H
