#ifndef PASSWORD_MANAGER_UTILS_H
#define PASSWORD_MANAGER_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class Utils {
public:
    Utils() = default;
    std::string base64_encode(const std::vector<uint8_t>&);
private:
    const std::string base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};

#endif //PASSWORD_MANAGER_UTILS_H
