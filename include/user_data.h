#ifndef PASSWORD_MANAGER_USER_DATA_H
#define PASSWORD_MANAGER_USER_DATA_H

#include <vector>
#include <string>
#include <cstdint>

class DataEntry {
public:
    void load_data();
    std::vector<uint8_t> serialize();
    void write_data_entry();
private:
    std::string user_name;
    std::string password;
    std::string notes;
    std::string date_created;
};

class UserData {
public:
    UserData() = default;
    void load_data();
    DataEntry at(size_t index);
    void insert(const DataEntry &data);
    std::vector<uint8_t> encrypt();
    void write_data();
private:
    std::vector<DataEntry> data;
    const uint64_t user_id;

    std::vector<uint8_t> serialize();
    std::vector<uint8_t> decrypt();
};

#endif //PASSWORD_MANAGER_USER_DATA_H
