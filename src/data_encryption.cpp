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

uint32_t AES::word_prod(uint32_t w1, uint32_t w2) {
    const uint8_t a3 = (w1 >> 24) & 0xff,
                  a2 = (w1 >> 16) & 0xff,
                  a1 = (w1 >> 8) & 0xff,
                  a0 = w1 & 0xff,
                  b3 = (w2 >> 24) & 0xff,
                  b2 = (w2 >> 16) & 0xff,
                  b1 = (w2 >> 8) & 0xff,
                  b0 = w2 & 0xff,
                  d0 = AES::ffield_mult(a0, b0) ^ AES::ffield_mult(a3, b1) ^ AES::ffield_mult(a2, b2) ^ AES::ffield_mult(a1, b3),
                  d1 = AES::ffield_mult(a1, b0) ^ AES::ffield_mult(a0, b1) ^ AES::ffield_mult(a3, b2) ^ AES::ffield_mult(a2, b3),
                  d2 = AES::ffield_mult(a2, b0) ^ AES::ffield_mult(a1, b1) ^ AES::ffield_mult(a0, b2) ^ AES::ffield_mult(a3, b3),
                  d3 = AES::ffield_mult(a3, b0) ^ AES::ffield_mult(a2, b1) ^ AES::ffield_mult(a1, b2) ^ AES::ffield_mult(a0, b3);
    return (d3 << 24) + (d2 << 16) + (d1 << 8) + d0;
}
