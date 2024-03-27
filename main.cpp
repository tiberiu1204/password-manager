#include "program.h"
#include "data_encryption.h"
#include <iostream>
#include <iomanip>
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
    std::vector<uint64_t> hash = SHA512().hash("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
                                               "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");
    for(auto &a : hash) {
        std::cout<<std::setfill('0')<<std::setw(16)<<std::hex<<a;
    }
    std::cout<<"\n";
    return Program::start_program("dev");
}
