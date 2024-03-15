#include <iostream>
#include "bcrypt.h"
#include <random>

int main() {
    Bcrypt bc;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,0xff);
    std::string salt(16, 0);
    for(int i = 0; i < 16; i++) {
        salt[i] = static_cast<char>(dist(rng));
    }
    std::string password = "cacamaca2leivaca";

    // TODO: fix

    std::cout<<bc.hash(password, salt, 12, 12);
}
