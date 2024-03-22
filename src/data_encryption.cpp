#include "data_encryption.h"

uint8_t AES::xtime(uint8_t byte, uint8_t times = 1) {
    if(times == 0) return byte;
    while(times--) {
        byte = (byte << 1) & 0x100 ? (byte << 1) ^ 0x11b : byte << 1;
    }
    return byte;
}

uint8_t AES::ffield_mult(uint8_t b1, uint8_t b2) {
    if(b1 < b2) std::swap(b1, b2);
    uint8_t result = 0;
    size_t bit_index = 0;
    while(b2) {
        result ^= AES::xtime(b1, bit_index++) * (b2 & 1);
        b2 >>= 1;
    }
    return result;
}