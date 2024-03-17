#include "user.h"
#include "bcrypt.h"

void User::add_user(const User &user) {

}

bool User::validate_password(std::string &password) {
    const std::string salt = Bcrypt::extract_salt(this->password_hash);
    const uint8_t cost = Bcrypt::extract_cost(this->password_hash);
    return this->password_hash == Bcrypt().hash(password, salt, cost);
}