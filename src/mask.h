#ifndef MASK_H
#define MASK_H
void mask_string(char* string, uint32_t key) {
	for(int i = 0; i < strlen(string); i++) {
		int key_octet_j = i % 4;
		string[i] ^= ((uint8_t*)&key)[key_octet_j];
	}
}
#endif
