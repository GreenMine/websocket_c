#ifndef GENERATOR_H
#define GENERATOR_H
uint8_t* generate_data_frame(uint8_t flag, uint8_t* data, size_t data_len, size_t* ret_len, bool mask) {
	//FRSV|OP| MASK|PAYLOAD LENGTH
	uint8_t *return_frame = malloc(data_len + 14);
	return_frame[0] = 128 | flag;

	size_t data_offset = 2;//FIN, RSV, OPCODE, MASK and LENGTH
	if(mask) data_offset += 4;
	//GET MESSAGE LENGTH
	if(data_len > 125) {
		if(data_len > 65535) {
			return_frame[1] = 127;
			*(uint64_t*)(return_frame + 2) = data_len;
			data_offset += 8;
			reverse_array(return_frame + 2, 8);
		} else {
			return_frame[1] = 126;
			*(uint16_t*)(return_frame + 2) = data_len;
			data_offset += 2;
			reverse_array(return_frame + 2, 2);
		}
	} else {
		return_frame[1] = data_len;
	}
	//ADD MASK BIT
	return_frame[1] |= (mask << 7);

	memcpy(return_frame + data_offset, data, data_len);
	if(mask) {
		uint32_t mask = 0x37fa213d;//TODO: About random - Randomness Requirements for Security RFC4086(https://tools.ietf.org/html/rfc4086)
		reverse_array(&mask, sizeof(mask));

		mask_data(return_frame + data_offset, data_len, mask);
		*(uint32_t*)&return_frame[data_offset - 4] = mask;
	}

	*ret_len = data_len + data_offset;
	return return_frame;
}
#endif
