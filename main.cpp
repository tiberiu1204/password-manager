#include "program.h"
#include "data_encryption.h"
#include <iostream>
int main() {
    std::string key;
    std::string plaintext;
    for(uint8_t i = 0; i < 16; i++) {
        plaintext.push_back(static_cast<char>(i * 0x11));
    }
    for(uint8_t i = 0; i < 12; i++) {
        plaintext.push_back(static_cast<char>(i * 0x11));
    }
    for(char i = 0; i < 32; i++) key.push_back(i);
    std::vector<uint8_t> out = AES256().encrypt(plaintext, key);
    for(auto &c : out) {
        std::cout<<std::hex<<static_cast<uint16_t>(c);
    }
//    std::cout<<std::hex<<AES256::word_prod(0x6353e08c, 0x03010102);
    //return Program::start_program("dev");
}
