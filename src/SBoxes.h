#pragma once

#include <cstdint>

extern uint8_t SBox_AES[256];

extern const uint8_t SBox_Midori128_S0[256];
extern const uint8_t SBox_Midori128_S1[256];
extern const uint8_t SBox_Midori128_S2[256];
extern const uint8_t SBox_Midori128_S3[256];

void init_SBox_AES(uint8_t sbox[256]);

inline void init_SBoxes() {
	init_SBox_AES(SBox_AES);
}
