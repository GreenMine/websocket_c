#include "helper.h"
void reverse_array(void* array, size_t length) {
	uint8_t* ptr = (uint8_t*)array;
	for(int i = 0; i < length / 2; i++) {
		uint8_t temp = ptr[i];
		ptr[i] = ptr[length-i-1];
		ptr[length-i-1] = temp;
	}
}

void mask_data(uint8_t* data, size_t data_len, uint32_t key) {
	for(int i = 0; i < data_len; i++) {
		int key_octet_j = i % 4;
		data[i] ^= ((uint8_t*)&key)[key_octet_j];
	}
}
