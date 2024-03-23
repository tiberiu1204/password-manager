#include "user.h"

#include <utility>
#include "bcrypt.h"

User::User(uint64_t user_id, std::string username, std::string date_created) :
user_id(user_id), username(std::move(username)), date_created(std::move(date_created)), data(UserData(user_id)) {}

void User::retrieve_data() {

}
