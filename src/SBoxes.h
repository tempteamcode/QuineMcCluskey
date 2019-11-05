#pragma once

#include <cstdint>

extern "C" void initialize_aes_sbox(uint8_t sbox[256]);

const auto fill_SBox_AES = initialize_aes_sbox;
