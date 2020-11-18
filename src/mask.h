#ifndef MASK_H
#define MASK_H
void mask_data(uint8_t* data, size_t data_len, uint32_t key) {
	for(int i = 0; i < data_len; i++) {
		int key_octet_j = i % 4;
		data[i] ^= ((uint8_t*)&key)[key_octet_j];
	}
}
#endif
