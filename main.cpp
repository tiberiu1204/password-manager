#include "program.h"
#include "data_encryption.h"
#include <iostream>
#include <iomanip>
#include "bcrypt.h"

int main() {
//    std::string key;
//    std::string plaintext;
//    for(uint8_t i = 0; i < 16; i++) {
//        plaintext.push_back(static_cast<char>(i * 0x11));
//    }
//    plaintext = "asdfasdfasdfasdfasdfasdfasfd";
//    for(char i = 0; i < 32; i++) key.push_back(i);
//    std::vector<uint8_t> out = AES256().encrypt(plaintext, key);
//    for(auto &c : out) {
//        std::cout<<std::hex<<static_cast<uint16_t>(c);
//    }
//    std::cout<<"\n-----------------DECRYPTION--------------------\n";
//    std::string out2 = AES256().decrypt(out, key);
//    for(auto &c : out2) {
//        std::cout<<std::hex<<static_cast<uint16_t>(c);
//    }
//    std::cout<<"\n"<<out2;
    //    std::cout<<std::hex<<AES256::word_prod(0x6353e08c, 0x03010102);
//    std::string str;
//    for(int i = 0; i < 1000000; i++) {
//        str.push_back('a');
//    }
//    std::vector<uint8_t> hash = SHA512().hash(str);
//    for(auto &a : hash) {
//        std::cout<<std::setfill('0')<<std::setw(2)<<std::hex<<static_cast<uint16_t>(a);
//    }
//    std::cout<<"\n";
//    std::vector<uint8_t> v = HMAC::apply_hmac("alabalaportocala", "asdfasdfasdf", std::make_unique<SHA512>());
//    for(const auto &ch : v) {
//        std::cout<<std::setfill('0')<<std::setw(2)<<std::hex<<static_cast<uint16_t>(ch);
//    }
////    std::cout<<"\n";
//    std::vector<uint8_t> v = PBKDF2::derive_key("parola123", Bcrypt::generate_salt(), 20000);
//    for(const auto &a : v) {
//        std::cout<<std::setfill('0')<<std::setw(2)<<std::hex<<static_cast<uint16_t>(a);
//    }
//    std::cout<<"\n"<<std::dec<<v.size()<<"\n";
    return Program::start_program("dev");
}
