#include "program.h"
#include "data_encryption.h"
#include <iostream>
int main() {
    std::cout<<static_cast<uint16_t>(AES::ffield_mult(0x57, 0x13));
    //return Program::start_program("dev");
}
