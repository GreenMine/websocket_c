#ifndef GENERATOR_H
#define GENERATOR_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
uint8_t* generate_data_frame(uint8_t flag, uint8_t* data, size_t data_len, size_t* ret_len, bool mask);
#endif
