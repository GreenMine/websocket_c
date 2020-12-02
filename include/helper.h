#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <stdint.h>
void reverse_array(void* array, size_t length);
void mask_data(uint8_t* data, size_t data_len, uint32_t key);
#endif
