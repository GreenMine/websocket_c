#ifndef HELPER_H
#define HELPER_H
void reverse_array(void* array, size_t length) {
	uint8_t* ptr = (uint8_t*)array;
	for(int i = 0; i < length / 2; i++) {
		uint8_t temp = ptr[i];
		ptr[i] = ptr[length-i-1];
		ptr[length-i-1] = temp;
	}
}
#endif
