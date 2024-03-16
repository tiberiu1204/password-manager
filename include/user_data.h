#ifndef PASSWORD_MANAGER_USER_DATA_H
#define PASSWORD_MANAGER_USER_DATA_H

#include <vector>
#include <cstdint>

class UserData {
public:
    UserData() = default;
private:
    std::vector<uint8_t> raw_data;
};

#endif //PASSWORD_MANAGER_USER_DATA_H
