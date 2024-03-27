#include "data_encryption.h"
#include <cassert>

std::vector<uint8_t> AES256::encrypt(const std::string &text, const std::vector<uint8_t> &master_key) {
    for(size_t i = 0; i < 32; i++) {
        this->key[i] = master_key[i];
    }
    this->key_expansion();
    size_t index = 0;
    std::vector<uint8_t> output;
    uint8_t padding_val = 0;
    while(index < text.size()) {
        size_t chr_ind = index;
        index += 16;
        uint8_t state[4][4];
        for(; chr_ind < index; chr_ind++) {
            if(chr_ind < text.size()) {
                state[(chr_ind % 16) % 4][(chr_ind % 16) / 4] = text.at(chr_ind);
            }
            else {
                state[(chr_ind % 16) % 4][(chr_ind % 16) / 4] = padding_val;
            }
        }
        this->cypher(state);
        for(size_t i = 0; i < 16; i++) {
            output.push_back(state[i % 4][i / 4]);
        }
    }
    return output;
}

std::string AES256::decrypt(const std::vector<uint8_t> &byte_arr, const std::vector<uint8_t> &master_key) {
    std::string output;
    for(int i = 0; i < 32; i++) {
        this->key[i] = static_cast<uint8_t>(master_key[i]);
    }
    this->key_expansion();
    size_t index = 0;
    while(index < byte_arr.size()) {
        size_t byte_ind = index;
        index += 16;
        uint8_t state[4][4];
        for(; byte_ind < index; byte_ind++) {
            state[(byte_ind % 16) % 4][(byte_ind % 16) / 4] = byte_arr.at(byte_ind);
        }
        this->inv_cypher(state);
        for(size_t i = 0; i < 16; i++) {
            char chr = static_cast<char>(state[i % 4][i / 4]);
            if(chr) {
                output.push_back(static_cast<char>(state[i % 4][i / 4]));
            }
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

void AES256::sub_bytes(uint8_t state[4][4], bool inverse = false) {
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            state[i][j] = inverse ? this->inv_S[state[i][j]] : this->S[state[i][j]];
        }
    }
}

void AES256::shift_rows(uint8_t state[4][4], bool inverse = false) {
    uint8_t temp_row[4];
    for(size_t i = 1; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            if(inverse) {
                temp_row[(j + i) & 3] = state[i][j];
            }
            else {
                temp_row[j] = state[i][(j + i) & 3];
            }
        }
        for(size_t j = 0; j < 4; j++) {
            state[i][j] = temp_row[j];
        }
    }
}

void AES256::mix_columns(uint8_t state[4][4], bool inverse = false) {
    for(size_t i = 0; i < 4; i++) {
        uint32_t word = 0;
        for(size_t j = 0; j < 4; j++) {
            word = word + (state[j][i] << (8 * j));
        }
        word = inverse ?
                AES256::word_prod(word, 0x0b0d090e) :
                AES256::word_prod(word, 0x03010102);
        state[3][i] = word >> 24;
        state[2][i] = (word >> 16) & 0xff;
        state[1][i] = (word >> 8) & 0xff;
        state[0][i] = word & 0xff;
    }
}

void AES256::add_round_key(uint8_t state[4][4], uint8_t round) {
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            state[j][i] = state[j][i] ^ ((this->exp_key[round * 4 + i] >> (24 - 8 * j)) & 0xff);
        }
    }
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

void AES256::inv_cypher(uint8_t state[4][4]) {
    this->add_round_key(state, 14);
    for(size_t round = 13; round > 0; round--) {
        AES256::shift_rows(state, true);
        this->sub_bytes(state, true);
        this->add_round_key(state, round);
        AES256::mix_columns(state, true);
    }
    AES256::shift_rows(state, true);
    this->sub_bytes(state, true);
    this->add_round_key(state, 0);
}

std::vector<uint8_t> SHA512::hash(const std::string &message) {
    std::string padded_mdg = SHA512::pad_message(message);
    std::vector<SHA512::Block> parsed_msg = SHA512::parse_message(padded_mdg);
    std::vector<uint64_t> curr_hash = SHA512::get_initial_hash();

    for(SHA512::Block block : parsed_msg) {
        uint64_t msg_sch[80];
        for(size_t i = 0; i < 80; i++) {
            if(i < 16) {
                msg_sch[i] = block.at(i);
            }
            else {
                msg_sch[i] = SHA512::sig1(msg_sch[i - 2]) + msg_sch[i - 7] +
                             SHA512::sig0(msg_sch[i - 15]) + msg_sch[i - 16];
            }
        }
        uint64_t a = curr_hash[0], b = curr_hash[1], c = curr_hash[2], d = curr_hash[3], e = curr_hash[4],
                 f = curr_hash[5], g = curr_hash[6], h = curr_hash[7];
        for(size_t i = 0; i < 80; i++) {
            uint64_t word1 = h + SHA512::bigsig1(e) + SHA512::ch(e, f, g) + this->K[i] + msg_sch[i],
                     word2 = SHA512::bigsig0(a) + SHA512::maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + word1;
            d = c;
            c = b;
            b = a;
            a = word1 + word2;
        }
        curr_hash[0] = a + curr_hash[0];
        curr_hash[1] = b + curr_hash[1];
        curr_hash[2] = c + curr_hash[2];
        curr_hash[3] = d + curr_hash[3];
        curr_hash[4] = e + curr_hash[4];
        curr_hash[5] = f + curr_hash[5];
        curr_hash[6] = g + curr_hash[6];
        curr_hash[7] = h + curr_hash[7];
    }
    std::vector<uint8_t> final_hash;
    for(size_t i = 0; i < 8; i++) {
        for(size_t j = 8; j > 0; j--) {
            final_hash.push_back((curr_hash[i] >> (8 * j - 8)) & 0xff);
        }
    }
    return final_hash;
}

uint64_t SHA512::rotr(uint64_t x, int32_t n) {
    return (x >> n) | (x << (64 - n));
}

uint64_t SHA512::ch(uint64_t x, uint64_t y, uint64_t z) {
    return (x & y) ^ ((~x) & z);
}

uint64_t SHA512::maj(uint64_t x, uint64_t y, uint64_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint64_t SHA512::bigsig0(uint64_t x) {
    return SHA512::rotr(x, 28) ^ SHA512::rotr(x, 34) ^ SHA512::rotr(x, 39);
}

uint64_t SHA512::bigsig1(uint64_t x) {
    return SHA512::rotr(x, 14) ^ SHA512::rotr(x, 18) ^ SHA512::rotr(x, 41);
}

uint64_t SHA512::sig0(uint64_t x) {
    return SHA512::rotr(x, 1) ^ SHA512::rotr(x, 8) ^ (x >> 7);
}

uint64_t SHA512::sig1(uint64_t x) {
    return SHA512::rotr(x, 19) ^ SHA512::rotr(x, 61) ^ (x >> 6);
}

std::string SHA512::pad_message(const std::string &message) {
    std::string padded_msg = message;
    uint64_t msg_len = padded_msg.size();
    int64_t k = (895 - static_cast<int64_t>(msg_len) * 8) % 1024;
    k = k < 0 ? k + 1024 : k;
    if(k > 0) padded_msg.push_back(static_cast<char>(0x80));
    for(size_t i = 0; i < k / 8; i++) {
        padded_msg.push_back(0);
    }
    for(size_t i = 0; i < 16; i++) {
        if(i < 9) padded_msg.push_back(0);
        else padded_msg.push_back(static_cast<char>(((msg_len * 8) >> (120 - 8 * i)) & 0xff));
    }
    assert(padded_msg.size() * 8 % 1024 == 0);
    return padded_msg;
}

std::vector<SHA512::Block> SHA512::parse_message(const std::string &message) {
    std::vector<Block> block_arr;
    uint64_t word = 0;
    Block block;
    for(size_t i = 0; i < message.size(); i++) {
        word <<= 8;
        word += static_cast<uint8_t>(message[i]);
        if((i + 1) % 8 == 0) {
            block.push_back(word);
            word = 0;
        }
        if((i + 1) % 128 == 0) {
            block_arr.push_back(block);
            assert(block.size() == 16);
            block.clear();
        }
    }
    assert(block_arr.size() == message.size() * 8 / 1024);
    return block_arr;
}

std::vector<uint64_t> SHA512::get_initial_hash() {
    return {
            0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
            0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
    };
}

constexpr uint32_t SHA512::get_input_block_size() const {
    return 128;
}

constexpr uint32_t SHA512::get_output_block_size() const {
    return 64;
}

std::vector<uint8_t> HMAC::apply_hmac(const std::string &text, const std::vector<uint8_t> &key, std::unique_ptr<SHA> sha) {
    uint32_t B = sha->get_input_block_size();
    uint32_t L = sha->get_output_block_size();
    std::vector<uint8_t> key0;
    if(key.size() == B) {
        key0.insert(key0.end(), key.begin(), key.end());
    }
    else if(key.size() > B) {
        std::string str_key;
        str_key.insert(str_key.begin(), key.begin(), key.end());
        key0 = sha->hash(str_key);
        for(size_t i = 0; i < B - L; i++) key0.push_back(0);
    }
    else {
        key0.insert(key0.end(), key.begin(), key.end());
        for(size_t i = 0; i < B - key.size(); i++) {
            key0.push_back(0);
        }
    }

    assert(key0.size() == B);

    std::vector<uint8_t> temp = key0;
    for(size_t i = 0; i < B; i++) {
        temp[i] ^= 0x36;
    }

    std::vector<uint8_t> result = temp;
    result.insert(result.end(), text.begin(), text.end());
    std::string result_str;
    result_str.insert(result_str.end(), result.begin(), result.end());

    assert(result.size() == text.size() + temp.size());

    result = sha->hash(result_str);

    assert(result.size() == L);

    for(size_t i = 0; i < B; i++) {
        key0[i] ^= 0x5c;
    }

    key0.insert(key0.end(), result.begin(), result.end());
    result_str.clear();
    result_str.insert(result_str.end(), key0.begin(), key0.end());
    result = sha->hash(result_str);

    assert(result.size() == L);

    return result;
}

std::vector<uint8_t> PBKDF2::derive_key(const std::string &password, const std::string &salt, const uint32_t iterations) {
    // TODO: Maybe handle errors better
    constexpr const uint64_t h_len = 512;
    constexpr const uint64_t k_len = 256;

    uint64_t len = (k_len - 1) / h_len + 1;
    uint64_t remainder = k_len - (len - 1) * h_len;

    std::vector<std::vector<uint8_t>> T(len, std::vector<uint8_t>());

    for(size_t i = 1; i <= len; i++) {
        std::vector<uint8_t> U;
        U.insert(U.begin(), salt.begin(), salt.end());
        U.push_back(i >> 24);
        U.push_back((i >> 16) & 0xff);
        U.push_back((i >> 8) & 0xff);
        U.push_back(i & 0xff);
        T[i - 1].resize(h_len / 8, 0);
        for(size_t j = 0; j < iterations; j++) {
            U = HMAC::apply_hmac(password, U, std::make_unique<SHA512>());
            for(size_t k = 0; k < U.size(); k++) {
                T[i - 1][k] ^= U[k];
            }
        }
    }
    (T.end() - 1)->resize(remainder / 8);
    std::vector<uint8_t> result;
    for(const auto &arr : T) {
        result.insert(result.end(), arr.begin(), arr.end());
    }
    return result;
}