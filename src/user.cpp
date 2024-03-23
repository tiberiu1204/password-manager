#include "user.h"

#include <utility>
#include "bcrypt.h"

User::User(uint64_t user_id, std::string username, std::string date_created) :
user_id(user_id), username(std::move(username)), date_created(std::move(date_created)), data(UserData(user_id)) {}

void User::retrieve_data() {

}

User::~User() {
    this->username.clear();
    this->date_created.clear();
}

User &User::operator=(const User &other) {
    this->username = other.username;
    this->date_created = other.date_created;
    return *this;
}

User::User(const User &other) :
user_id(other.user_id), username(other.username), date_created(other.date_created), data(other.data) {}
