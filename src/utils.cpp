#include "utils.h"

// TODO: Error handling

void d() {}

std::string Utils::base64_encode(const std::vector<uint8_t> &byte_arr) {
    size_t index = 0;
    uint8_t bit_index = 0;
    std::string result;
    while(index < byte_arr.size()) {
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
            table_index += byte_arr.at(index + 1) >> (10 - bit_index) & 0b111111;
            index++;
            bit_index -= 2;
            result += this->base64_table[table_index];
        }
    }
    return result;
}