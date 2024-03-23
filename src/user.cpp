#include "user.h"

#include <utility>
#include "bcrypt.h"

User::User(uint64_t user_id, std::string username) :
user_id(user_id), username(std::move(username)), data(UserData(user_id)) {}

void User::retrieve_data() {

}
