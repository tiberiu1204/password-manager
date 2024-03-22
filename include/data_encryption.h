#ifndef PASSWORD_MANAGER_DATA_ENCRYPTION_H
#define PASSWORD_MANAGER_DATA_ENCRYPTION_H

#include <string>
#include <cstdint>

class AES {
public:
    std::string encrypt();
    std::string decrypt();
    // TODO: Change in private after done implementing;
public:
    //uint32_t round_const[];
    void add_round_key();
    void inv_mix_columns();
    void inv_shift_rows();
    void inv_sub_bytes();
    void mix_columns();
    void rot_word();
    void shift_rows();
    void sub_bytes();
    void sub_word();
    static uint8_t xtime(uint8_t, uint8_t);
    static uint8_t ffield_mult(uint8_t, uint8_t);
};

#endif //PASSWORD_MANAGER_DATA_ENCRYPTION_H
