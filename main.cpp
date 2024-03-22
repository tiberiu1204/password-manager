#include "program.h"
#include "data_encryption.h"
#include <iostream>
int main() {
    std::cout<<AES::word_prod(0x03010102, 0x0b0d090e);
    //return Program::start_program("dev");
}
