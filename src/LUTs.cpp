
const char LUT_hex_16[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
char LUT_hex_256[256][4];

const unsigned char LUT_bitscount_16[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
unsigned char LUT_bitscount_256[256];

void init_LUT_hex()
{
	char* chars = &LUT_hex_256[0][0];
	unsigned char value = 0;
	do {
		chars[0] = LUT_hex_16[(value & 0b11110000) >> 4];
		chars[1] = LUT_hex_16[(value & 0b00001111)];
		chars[2] = '\0';
		chars += sizeof(LUT_hex_256[0]);
	} while (++value);
}

void init_LUT_bitscount()
{
	LUT_bitscount_256[0] = 0;
	for (int g = 1; g < 256; g <<= 1)
	{
		for (int i = 0; i < g; ++i)
		{
			LUT_bitscount_256[g + i] = LUT_bitscount_256[i] + 1;
		}
	}
}

template <typename int_t>
unsigned char bitscount(const int_t value)
{
	const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&value);
	unsigned char result;
	int size = sizeof(int_t);
	while (size --> 0)
	{
		result += LUT_bitscount_256[*ptr++];
	}
	return result;
}

void init_LUTs()
{
	init_LUT_hex();
	init_LUT_bitscount();
}
