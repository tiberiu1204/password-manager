#include "bcrypt.h"

// TODO: Error handling (check for parameter size, etc.) and finish hash function

uint32_t Bcrypt::blowfish_f(uint32_t val) {
    uint32_t first_add_result = this->S[0][val >> 24] + this->S[1][val >> 16 & 0xff];
    return (first_add_result ^ this->S[2][val >> 8 & 0xff]) + this->S[3][val & 0xff];
}

uint64_t Bcrypt::blowfish_encrypt(uint64_t p_text) {
    uint32_t left = p_text >> 32;
    uint32_t right = p_text & 0xffffffff;
    for(size_t i = 0; i < 16; i++) {
        left ^= this->P[i];
        right ^= this->blowfish_f(left);
        std::swap(left, right);
    }
    std::swap(left, right);
    right ^= this->P[16];
    left^= this->P[17];
    return (static_cast<uint64_t>(left) << 32) + right;
}

void Bcrypt::expand_key(const std::vector<uint8_t> &password, const std::vector<uint8_t> &salt) {
    for(size_t i = 0; i < 18; i++) {
        this->P[i] ^= password[i % password.size()];
    }
    uint64_t salt_half[2] = {};
    for(size_t i = 0; i < 8; i++) {
        salt_half[0] = (salt_half[0] << 8) + salt[i];
        salt_half[1] = (salt_half[1] << 8) + salt[i + 8];
    }
    uint64_t block = 0;
    for(size_t i = 0; i < 9; i++) {
        block ^= salt_half[i % 2];
        block = this->blowfish_encrypt(block);
        this->P[2 * i] = block & 0xffffffff;
        this->P[2 * i + 1] = block >> 32;
    }
    for(auto & subst_box : this->S) {
        for(size_t i = 0; i < 127; i++) {
            block ^= salt_half[i % 2];
            block = this->blowfish_encrypt(block);
            subst_box[i * 2] = block & 0xffffffff;
            subst_box[i * 2  + 1] = block >> 32;
        }
    }
}

void Bcrypt::eks_blowfish_setup(const std::vector<uint8_t> &password, const std::vector<uint8_t> &salt, uint8_t cost) {
    this->expand_key(password, salt);
    cost = 1 << cost;
    std::vector<uint8_t> null_salt(16, 0);
    while(cost--) {
        this->expand_key(password, null_salt);
        this->expand_key(salt, null_salt);
    }
}

std::vector<uint8_t> Bcrypt::hash(std::vector<uint8_t> &password, const std::vector<uint8_t> &salt,
                                  uint8_t cost, uint8_t iterations) {
    this->eks_blowfish_setup(password, salt, cost);
    while(iterations--) {

    }
}

