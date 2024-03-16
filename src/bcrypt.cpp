#include "bcrypt.h"
#include <unordered_map>

// TODO: Error handling (check for parameter size, etc.) and finish hash function

std::vector<uint8_t> Bcrypt::extract_salt(const std::string &bcrypt_hash) {
    const std::string base64_table = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::unordered_map<char, int> index_map;
    for(int i = 0; i < base64_table.size(); i++) {
        index_map[base64_table[i]] = i;
    }
    std::vector<uint8_t> salt(16, 0);
    size_t h_index = 7;
    size_t bit_index = 0;
    for(size_t i = 0; i < 16; i++) {
        uint8_t byte = 0;
        for(size_t j = 0; j < 8; j++) {
            byte = (byte << 1) + ((index_map[bcrypt_hash.at(h_index)] >> (5 - bit_index++)) & 1);
            if(bit_index == 6) {
                bit_index = 0;
                h_index++;
            }
        }
        salt[i] = byte;
    }
    return salt;
}

std::string Bcrypt::base64_encode(const std::string &byte_arr) {
    size_t index = 0;
    uint8_t bit_index = 0;
    std::string result;
    while(index < byte_arr.size() && index < 23) {
        if(bit_index <= 2) {
            result += this->base64_table[byte_arr.at(index) >> (2 - bit_index) & 0b111111];
            bit_index += 6;
            if(bit_index == 8) {
                index++;
                bit_index = 0;
            }
        }
        else {
            uint8_t table_index = byte_arr.at(index) & ((1 << (8 - bit_index)) - 1);
            table_index <<= bit_index - 2;
            if(index + 1 < byte_arr.size()) {

                table_index += static_cast<uint8_t>(byte_arr.at(index + 1)) >> (10 - bit_index) & 0b111111;
                bit_index -= 2;
            }
            result += this->base64_table.at(table_index);
            index++;
        }
    }
    return result;
}

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

    std::string Bcrypt::blowfish_encrypt_ecb(const std::string& p_text) {
    std::string c_text(p_text.size(), 0);
    size_t c_text_index = 0;
    uint64_t block = 0;
    for(size_t i = 0; i < p_text.size(); i++) {
        block <<= 8;
        block += p_text[i];
        if(i % 8 == 7) {
            block = this->blowfish_encrypt(block);
            for(int j = 0; j < 8; j++) {
                c_text.at(c_text_index++) = static_cast<char>(block >> (56 - 8 * j) & 0xff);
            }
            block = 0;
        }
    }
    if(c_text_index < c_text.size()) {
        block = this->blowfish_encrypt(block);
        while(c_text_index < c_text.size()) {
            c_text.at(c_text_index++) = static_cast<char>(block >> ((c_text.size() - c_text_index - 1) * 8) & 0xff);
        }
    }
    return c_text;
}

void Bcrypt::expand_key(const std::string &password, const std::string &salt) {
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

void Bcrypt::eks_blowfish_setup(const std::string &password, const std::string &salt, uint8_t cost) {
    this->expand_key(password, salt);
    uint32_t iterations = 1 << cost;
    std::string null_salt(16, 0);
    while(iterations--) {
        this->expand_key(password, null_salt);
        this->expand_key(salt, null_salt);
    }
}

std::string Bcrypt::hash(std::string &password, const std::string &salt, uint8_t cost) {
    this->eks_blowfish_setup(password, salt, cost);
    uint32_t iterations = 1 << cost;
    std::string c_text = "This24ByteTextWillBeHash";
    while(iterations--) {
        c_text = this->blowfish_encrypt_ecb(c_text);
    }
    std::string salt_64 = this->base64_encode(salt);
    std::string c_text_64 = this->base64_encode(c_text);
    /* std::cout<<"Salt: "<<salt_64<<" with "<<salt_64.size()<<" chars.\n";
    std::cout<<"Hash: "<<c_text_64<<" with "<<c_text_64.size()<<" chars.\n";*/
    std::string result_hash = "$2t$" + std::to_string(cost) + "$" + salt_64 + c_text_64;
    return result_hash;
}

