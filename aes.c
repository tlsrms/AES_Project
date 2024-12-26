#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// AES S-box
static const uint8_t sbox[256] = {
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

// 역 S-box
static const uint8_t inv_sbox[256] = {
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// Round constant
static const uint8_t rcon[10] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};

// AES 상태 배열 정의 (4x4 행렬)
typedef uint8_t state_t[4][4];

void HexStringToByteArray(const char* hexString, uint8_t* byteArray) {
	for (int i = 0; i < 16; i++) {
		// 두 자리씩 읽어서 변환
		char byteStr[3] = { hexString[i * 2], hexString[i * 2 + 1], '\0' };
		byteArray[i] = (uint8_t)strtol(byteStr, NULL, 16);
	}
}

// 키 확장 함수
void KeyExpansion(const uint8_t* prev_key, uint8_t* next_key, int round) {
	uint8_t temp[4];

	// 마지막 4바이트를 temp에 복사 (RotWord 수행)
	temp[0] = prev_key[13];
	temp[1] = prev_key[14];
	temp[2] = prev_key[15];
	temp[3] = prev_key[12];

	// SubWord 수행 (S-box 적용)
	for (int i = 0; i < 4; i++) {
		temp[i] = sbox[temp[i]];
	}

	// 첫 번째 바이트에 Rcon 값 XOR 적용
	temp[0] ^= rcon[round - 1];

	// 새로운 키 생성
	for (int i = 0; i < 16; i++) {
		if (i < 4) {
			next_key[i] = prev_key[i] ^ temp[i];
		}
		else {
			next_key[i] = prev_key[i] ^ next_key[i - 4];
		}
	}
}

// AddRoundKey 함수
void AddRoundKey(state_t* state, const uint8_t* roundKey) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			(*state)[i][j] ^= roundKey[i + j * 4];
}

// SubBytes 함수
void SubBytes(state_t* state) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			(*state)[i][j] = sbox[(*state)[i][j]];
}

// ShiftRows 함수
void ShiftRows(state_t* state) {
	uint8_t temp;

	// 두 번째 행: 1 바이트 왼쪽 순환 이동
	temp = (*state)[1][0];
	(*state)[1][0] = (*state)[1][1];
	(*state)[1][1] = (*state)[1][2];
	(*state)[1][2] = (*state)[1][3];
	(*state)[1][3] = temp;

	// 세 번째 행: 2 바이트 왼쪽 순환 이동
	temp = (*state)[2][0];
	(*state)[2][0] = (*state)[2][2];
	(*state)[2][2] = temp;
	temp = (*state)[2][1];
	(*state)[2][1] = (*state)[2][3];
	(*state)[2][3] = temp;

	// 네 번째 행: 3 바이트 왼쪽 순환 이동 (혹은 1 바이트 오른쪽 순환 이동)
	temp = (*state)[3][3];
	(*state)[3][3] = (*state)[3][2];
	(*state)[3][2] = (*state)[3][1];
	(*state)[3][1] = (*state)[3][0];
	(*state)[3][0] = temp;
}

// AES의 Galois Field에서 곱셈 함수
uint8_t gmul(uint8_t a, uint8_t b) {
	uint8_t result = 0;
	while (b) {
		if (b & 1)
			result ^= a;  // XOR 연산
		uint8_t high_bit_set = a & 0x80;
		a <<= 1;
		if (high_bit_set)
			a ^= 0x1b;  // AES에서 사용하는 다항식 x^8 + x^4 + x^3 + x + 1
		b >>= 1;
	}
	return result;
}

// MixColumns 함수
void  MixColumns(state_t* state) {
	uint8_t temp_state[4][4];

	for (int c = 0; c < 4; c++) {
		temp_state[0][c] = gmul(0x02, (*state)[0][c]) ^ gmul(0x03, (*state)[1][c]) ^ (*state)[2][c] ^ (*state)[3][c];
		temp_state[1][c] = (*state)[0][c] ^ gmul(0x02, (*state)[1][c]) ^ gmul(0x03, (*state)[2][c]) ^ (*state)[3][c];
		temp_state[2][c] = (*state)[0][c] ^ (*state)[1][c] ^ gmul(0x02, (*state)[2][c]) ^ gmul(0x03, (*state)[3][c]);
		temp_state[3][c] = gmul(0x03, (*state)[0][c]) ^ (*state)[1][c] ^ (*state)[2][c] ^ gmul(0x02, (*state)[3][c]);
	}

	// 결과를 원래 상태 배열에 복사
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			(*state)[r][c] = temp_state[r][c];
}

// AES-128 암호화 함수
void AES_encrypt(const uint8_t* input, uint8_t* output, const uint8_t* key) {
	state_t state;
	uint8_t prev_roundKey[16];
	uint8_t next_roundKey[16];

	// 초기 상태 설정
	for (int i = 0; i < 16; i++)
		state[i % 4][i / 4] = input[i];

	// 초기 라운드 키 설정
	for (int i = 0; i < 16; i++)
		prev_roundKey[i] = key[i];

	// 초기 AddRoundKey
	AddRoundKey(&state, prev_roundKey);

	// 9번 라운드
	for (int round = 1; round < 10; round++) {
		SubBytes(&state);

		ShiftRows(&state);

		MixColumns(&state);

		KeyExpansion(prev_roundKey, next_roundKey, round);
		AddRoundKey(&state, next_roundKey);

		// 다음 라운드 키를 prev_roundKey로 설정
		for (int i = 0; i < 16; i++)
			prev_roundKey[i] = next_roundKey[i];
	}

	// 마지막 라운드
	SubBytes(&state);

	ShiftRows(&state);

	// 마지막 라운드 키 생성
	KeyExpansion(prev_roundKey, next_roundKey, 10);
	AddRoundKey(&state, next_roundKey);

	// 출력
	for (int i = 0; i < 16; i++)
		output[i] = state[i % 4][i / 4];
}

// Inverse ShiftRows
void InvShiftRows(state_t* state) {
	uint8_t temp;

	// 두 번째 행: 1 바이트 오른쪽 순환 이동
	temp = (*state)[1][3];
	(*state)[1][3] = (*state)[1][2];
	(*state)[1][2] = (*state)[1][1];
	(*state)[1][1] = (*state)[1][0];
	(*state)[1][0] = temp;

	// 세 번째 행: 2 바이트 오른쪽 순환 이동
	temp = (*state)[2][0];
	(*state)[2][0] = (*state)[2][2];
	(*state)[2][2] = temp;
	temp = (*state)[2][1];
	(*state)[2][1] = (*state)[2][3];
	(*state)[2][3] = temp;

	// 네 번째 행: 3 바이트 오른쪽 순환 이동
	temp = (*state)[3][0];
	(*state)[3][0] = (*state)[3][1];
	(*state)[3][1] = (*state)[3][2];
	(*state)[3][2] = (*state)[3][3];
	(*state)[3][3] = temp;
}

// Inverse SubBytes
void InvSubBytes(state_t* state) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			(*state)[i][j] = inv_sbox[(*state)[i][j]];
}

// Inverse MixColumns
void InvMixColumns(state_t* state) {
	uint8_t temp_state[4][4];

	for (int c = 0; c < 4; c++) {
		temp_state[0][c] = gmul(0x0e, (*state)[0][c]) ^ gmul(0x0b, (*state)[1][c]) ^ gmul(0x0d, (*state)[2][c]) ^ gmul(0x09, (*state)[3][c]);
		temp_state[1][c] = gmul(0x09, (*state)[0][c]) ^ gmul(0x0e, (*state)[1][c]) ^ gmul(0x0b, (*state)[2][c]) ^ gmul(0x0d, (*state)[3][c]);
		temp_state[2][c] = gmul(0x0d, (*state)[0][c]) ^ gmul(0x09, (*state)[1][c]) ^ gmul(0x0e, (*state)[2][c]) ^ gmul(0x0b, (*state)[3][c]);
		temp_state[3][c] = gmul(0x0b, (*state)[0][c]) ^ gmul(0x0d, (*state)[1][c]) ^ gmul(0x09, (*state)[2][c]) ^ gmul(0x0e, (*state)[3][c]);
	}

	// 결과를 원래 상태 배열에 복사
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			(*state)[r][c] = temp_state[r][c];
}

// 이전 라운드 키를 계산하는 함수
void InvKeyExpansion(const uint8_t* curr_round_key, uint8_t* prev_round_key, int round) {
	uint8_t w0[4], w1[4], w2[4], w3[4];
	uint8_t w4[4], w5[4], w6[4], w7[4];
	uint8_t temp[4];
	uint8_t rot_word[4];
	uint8_t rcon_val;

	// 현재 라운드 키를 4개의 워드로 분할
	for (int i = 0; i < 4; i++) {
		w4[i] = curr_round_key[i];
		w5[i] = curr_round_key[i + 4];
		w6[i] = curr_round_key[i + 8];
		w7[i] = curr_round_key[i + 12];
	}

	// w3 = w6 XOR w7
	for (int i = 0; i < 4; i++)
		w3[i] = w6[i] ^ w7[i];

	// w2 = w5 XOR w6
	for (int i = 0; i < 4; i++)
		w2[i] = w5[i] ^ w6[i];

	// w1 = w4 XOR w5
	for (int i = 0; i < 4; i++)
		w1[i] = w4[i] ^ w5[i];

	// RotWord(w3)
	rot_word[0] = w3[1];
	rot_word[1] = w3[2];
	rot_word[2] = w3[3];
	rot_word[3] = w3[0];

	// SubWord(RotWord(w3))
	for (int i = 0; i < 4; i++)
		temp[i] = sbox[rot_word[i]];

	// Rcon 적용
	rcon_val = rcon[round - 1];
	temp[0] = temp[0] ^ rcon_val;

	// w0 = w4 XOR temp
	for (int i = 0; i < 4; i++)
		w0[i] = w4[i] ^ temp[i];

	// 이전 라운드 키 구성: prev_round_key = {w0, w1, w2, w3}
	for (int i = 0; i < 4; i++) {
		prev_round_key[i] = w0[i];
		prev_round_key[i + 4] = w1[i];
		prev_round_key[i + 8] = w2[i];
		prev_round_key[i + 12] = w3[i];
	}
}

// AES-128 복호화 함수
void AES_decrypt(const uint8_t* input, uint8_t* output, const uint8_t* key) {
	state_t state;
	uint8_t roundKey[16];
	uint8_t prev_roundKey[16];
	uint8_t next_Key[16];

	// 초기 상태 설정
	for (int i = 0; i < 16; i++)
		state[i % 4][i / 4] = input[i];
	
	for (int i = 0; i < 16; i++)
		prev_roundKey[i] = key[i];
	for (int i = 1; i <= 10; i++) {
		KeyExpansion(prev_roundKey, roundKey, i);
		for (int i = 0; i < 16; i++)
			prev_roundKey[i] = roundKey[i];
	}
	// 마지막 라운드 역연산
	AddRoundKey(&state, roundKey);

	InvShiftRows(&state);

	InvSubBytes(&state);

	// 중간 라운드 역연산
	for (int round = 9; round > 0; round--) {
		// 이전 라운드 키 생성
		InvKeyExpansion(roundKey, prev_roundKey, round + 1);
		AddRoundKey(&state, prev_roundKey);
		InvMixColumns(&state);
		// 현재 라운드 키를 업데이트
		for (int i = 0; i < 16; i++)
			roundKey[i] = prev_roundKey[i];
		InvShiftRows(&state);
		InvSubBytes(&state);
	}

	// 초기 라운드 역연산
	AddRoundKey(&state, key);

	// 출력
	for (int i = 0; i < 16; i++)
		output[i] = state[i % 4][i / 4];
}

int main() {
	char inputHex[33];  // 32자리 16진수 + NULL 문자
	char keyHex[33];
	uint8_t input[16];
	uint8_t key[16];
	uint8_t encrypted[16];
	uint8_t decrypted[16];

	printf("Enter Plain Text (32 hex digits): ");
	scanf("%32s", inputHex);

	printf("Enter Key (32 hex digits): ");
	scanf("%32s", keyHex);

	HexStringToByteArray(inputHex, input);
	HexStringToByteArray(keyHex, key);

	// 암호화
	AES_encrypt(input, encrypted, key);

	printf("Encrypted output: \n");
	for (int i = 0; i < 16; i++)
		printf("%02x", encrypted[i]);
	printf("\n");

	 //복호화
	 AES_decrypt(encrypted, decrypted, key);

	 printf("Decrypted output: \n");
	 for (int i = 0; i < 16; i++)
	     printf("%02x", decrypted[i]);
	 printf("\n");

	return 0;
}