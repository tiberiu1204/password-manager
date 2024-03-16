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
        //std::cout<<static_cast<int16_t>(salt[i])<<", ";
    }
    std::cout<<"\n";
    std::string password = "deffnotapassword";

    std::string hash = bc.hash(password, salt, 12);

    //std::cout<<Bcrypt::extract_salt(hash)<<"\n"<<salt;
    std::cout<<"Insert password:";
    std::cin>>password;
    std::string hash2 = Bcrypt().hash(password, Bcrypt::extract_salt(hash), 12);
    if(hash2== hash) std::cout<<"Correct.\n";
    else std::cout<<"Incorrect.";
}
