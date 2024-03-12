#ifndef PASSWORD_MANAGER_BCRYPT_H
#define PASSWORD_MANAGER_BCRYPT_H

#include <string>

class Bcrypt {
public:
    static std::string hash();
};

#endif //PASSWORD_MANAGER_BCRYPT_H
