#include "data_encryption.h"
#include <iostream>

std::string AES256::encrypt(const std::string &text, const std::string &password) {
    // TODO: convert password to key
    for(size_t i = 0; i < 32; i++) {
        this->key[i] = static_cast<uint8_t>(password[i]);
    }
    this->key_expansion();
    size_t index = 0;
    std::string output;
    uint8_t padding_val = text.size() % 16;
    while(index < text.size()) {
        size_t chr_ind = index;
        index += 16;
        uint8_t state[4][4];
        for(; chr_ind < index; chr_ind++) {
            if(chr_ind < text.size()) {
                state[(chr_ind % 16) % 4][(chr_ind % 16) / 4] = text[chr_ind];
            }
            else {
                state[(chr_ind % 16) % 4][(chr_ind % 16) / 4] = padding_val;
            }
        }
        this->cypher(state);
        for(size_t i = 0; i < 16; i++) {
            output.push_back(static_cast<char>(state[i % 4][i / 4]));
        }
    }
    return output;
}

uint8_t AES256::xtime(uint8_t byte, uint8_t times = 1) {
    if(times == 0) return byte;
    while(times--) {
        byte = (byte << 1) & 0x100 ? (byte << 1) ^ 0x11b : byte << 1;
    }
    return byte;
}

uint8_t AES256::ffield_mult(uint8_t b1, uint8_t b2) {
    if(b1 < b2) std::swap(b1, b2);
    uint8_t result = 0;
    size_t bit_index = 0;
    while(b2) {
        result ^= AES256::xtime(b1, bit_index++) * (b2 & 1);
        b2 >>= 1;
    }
    return result;
}

uint32_t AES256::word_prod(uint32_t w1, uint32_t w2) {
    const uint8_t a0 = w1 >> 24,
                  a1 = (w1 >> 16) & 0xff,
                  a2 = (w1 >> 8) & 0xff,
                  a3 = w1 & 0xff,
                  b0 = w2 >> 24,
                  b1 = (w2 >> 16) & 0xff,
                  b2 = (w2 >> 8) & 0xff,
                  b3 = w2 & 0xff,
                  d0 = AES256::ffield_mult(a0, b3) ^ AES256::ffield_mult(a1, b2) ^ AES256::ffield_mult(a2, b1) ^ AES256::ffield_mult(a3, b0),
                  d1 = AES256::ffield_mult(a1, b3) ^ AES256::ffield_mult(a2, b2) ^ AES256::ffield_mult(a3, b1) ^ AES256::ffield_mult(a0, b0),
                  d2 = AES256::ffield_mult(a2, b3) ^ AES256::ffield_mult(a3, b2) ^ AES256::ffield_mult(a0, b1) ^ AES256::ffield_mult(a1, b0),
                  d3 = AES256::ffield_mult(a3, b3) ^ AES256::ffield_mult(a0, b2) ^ AES256::ffield_mult(a1, b1) ^ AES256::ffield_mult(a2, b0);
    return (d0 << 24) + (d1 << 16) + (d2 << 8) + d3;
}

void print_state(uint8_t state[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            std::cout<<std::hex<<static_cast<unsigned>(state[j][i]);
        }
    }
    std::cout<<"\n";
}

void AES256::sub_bytes(uint8_t state[4][4]) {
    std::cout<<"Sub bytes\n";
    std::cout<<"Before\n";
    print_state(state);
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            state[j][i] = this->S[state[j][i]];
        }
    }
    std::cout<<"After\n";
    print_state(state);
}
/*
std::cout<<"Sub bytes\n";
std::cout<<"Before\n";
print_state(state);
std::cout<<"After\n";
print_state(state);
*/

void AES256::shift_rows(uint8_t state[4][4]) {
    std::cout<<"Shift rows\n";
    std::cout<<"Before\n";
    print_state(state);
    uint8_t temp_row[4];
    for(size_t i = 1; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            temp_row[j] = state[i][(j + i) & 3];
        }
        for(size_t j = 0; j < 4; j++) {
            state[i][j] = temp_row[j];
        }
    }
    std::cout<<"After\n";
    print_state(state);
}

// TODO: Delete debugging prints after implementation

void AES256::mix_columns(uint8_t state[4][4]) {
    std::cout<<"Mix columns\n";
    std::cout<<"Before\n";
    print_state(state);
    for(size_t i = 0; i < 4; i++) {
        uint32_t word = 0;
        for(size_t j = 0; j < 4; j++) {
            word = word + (state[j][i] << (8 * j));
        }
        word = AES256::word_prod(word, 0x03010102);
        state[3][i] = word >> 24;
        state[2][i] = (word >> 16) & 0xff;
        state[1][i] = (word >> 8) & 0xff;
        state[0][i] = word & 0xff;
    }
    std::cout<<"After\n";
    print_state(state);
}

void AES256::add_round_key(uint8_t state[4][4], uint8_t round) {
    std::cout<<"Add round key\n";
    std::cout<<"Before\n";
    print_state(state);
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            state[j][i] = state[j][i] ^ ((this->exp_key[round * 4 + i] >> (24 - 8 * j)) & 0xff);
        }
    }
    std::cout<<"After\n";
    print_state(state);
}

uint32_t AES256::sub_word(uint32_t word) {
    uint8_t b0 = this->S[word >> 24],
            b1 = this->S[(word >> 16) & 0xff],
            b2 = this->S[(word >> 8) & 0xff],
            b3 = this->S[word & 0xff];
    return (b0 << 24) + (b1 << 16) + (b2 << 8) + b3;
}

uint32_t AES256::rot_word(uint32_t word) {
    uint8_t b0 = word >> 24,
            b1 = (word >> 16) & 0xff,
            b2 = (word >> 8) & 0xff,
            b3 = word & 0xff;
    return (b1 << 24) + (b2 << 16) + (b3 << 8) + b0;
}

void AES256::key_expansion() {
    for(size_t i = 0; i < 8; i++) {
        this->exp_key[i] = (this->key[i * 4] << 24) + (this->key[i * 4 + 1] << 16) +
                           (this->key[i * 4 + 2] << 8) + (this->key[i * 4 + 3]);
    }
    uint32_t temp;
    for(size_t i = 8; i < 60; i++) {
        temp = this->exp_key[i - 1];
        if(i % 8 == 0) {
            temp = this->sub_word(AES256::rot_word(temp)) ^ this->rcon[i / 8];
        }
        else if(i % 8 == 4) {
            temp = this->sub_word(temp);
        }
        this->exp_key[i] = this->exp_key[i - 8] ^ temp;
    }
}

void AES256::cypher(uint8_t state[4][4]) {
    this->add_round_key(state, 0);
    for(uint8_t round = 1; round < 14; round++) {
        this->sub_bytes(state);
        AES256::shift_rows(state);
        AES256::mix_columns(state);
        this->add_round_key(state, round);
    }
    this->sub_bytes(state);
    AES256::shift_rows(state);
    this->add_round_key(state, 14);
}
