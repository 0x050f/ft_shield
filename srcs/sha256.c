#include "ft_shield.h"

#define RIGHTROTATE(x, y) (((x) >> (y)) | ((x) << (32 - (y))))

// https://en.wikipedia.org/wiki/SHA-2
char		*sha256(char *str, size_t size)
{
	uint64_t padding_zeroes;
	// setup msg
	//             v 0x80     v size
	// 64 bytes - 1 bytes - 8 bytes
	if (size % 64 > 55)
		padding_zeroes = 64 - ((size % 64) + 1) + 56;
	else
		padding_zeroes = 64 - ((size % 64) + 1) - 8;
	uint64_t	new_size = size + padding_zeroes + 1 + 8;
	uint8_t *msg = malloc(sizeof(char) * new_size);
	if (!msg)
		return (NULL);
	memcpy(msg, str, size);
	msg[size] = 0x80; // 0b10000000
	memset(msg + size + 1, 0, padding_zeroes);
	uint64_t	bit_length = size * 8;
	for (uint64_t i = 0; i < 8; i++) // as big-endian
		msg[new_size - (i + 1)] = bit_length >> 8 * i;

	//setup constant
	uint32_t h_array[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
				0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	uint32_t K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
					0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
					0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
					0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
					0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
					0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
					0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
					0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
					0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
					0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
					0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
					0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
					0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
					0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
					0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
					0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	for (uint8_t *chunk = msg; (uint64_t)(chunk - msg) < new_size; chunk += 64)
	{
		uint32_t w[64];
		int j = 0;
		for (int i = 0; i < 16; i++)
		{
			w[i] = (uint32_t) chunk[0 + j] << 24 | (uint32_t) chunk[1 + j] << 16 | (uint32_t) chunk[2 + j] << 8 | (uint32_t) chunk[3 + j];
			j += 4;
		}
		for (int i = 16; i < 64; i++)
		{
			uint32_t s0 = (RIGHTROTATE(w[i - 15], 7)) ^ (RIGHTROTATE(w[i - 15], 18)) ^ (w[i - 15] >> 3);
			uint32_t s1 = (RIGHTROTATE(w[i - 2], 17)) ^ (RIGHTROTATE(w[i - 2], 19)) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		uint32_t a = h_array[0];
		uint32_t b = h_array[1];
		uint32_t c = h_array[2];
		uint32_t d = h_array[3];
		uint32_t e = h_array[4];
		uint32_t f = h_array[5];
		uint32_t g = h_array[6];
		uint32_t h = h_array[7];

		for (int i = 0; i < 64; i++)
		{
			uint32_t S1 = (RIGHTROTATE(e, 6)) ^ (RIGHTROTATE(e, 11)) ^ (RIGHTROTATE(e, 25));
			uint32_t ch = (e & f) ^ ((~e) & g);
			uint32_t temp1 = h + S1 + ch + K[i] + w[i];
			uint32_t S0 = (RIGHTROTATE(a, 2)) ^ (RIGHTROTATE(a, 13)) ^ (RIGHTROTATE(a, 22));
			uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
			uint32_t temp2 = S0 + maj;
			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}
		h_array[0] += a;
		h_array[1] += b;
		h_array[2] += c;
		h_array[3] += d;
		h_array[4] += e;
		h_array[5] += f;
		h_array[6] += g;
		h_array[7] += h;
	}
	free(msg);
	char *hash = malloc(sizeof(char) * 65);
	if (!hash)
	{
		dprintf(STDERR_FILENO, "%s: malloc error\n", PRG_NAME);
		return (NULL);
	}
	for (int i = 0; i < 8; i++)
		sprintf(hash + 8 * i, "%08x", h_array[i]);
	hash[64] = '\0';
	return (hash);
}
