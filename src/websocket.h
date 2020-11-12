#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdlib.h>
#include <stdbool.h>

//TODO: Change sockfd to ws struct
void ws_send_message(int sockfd, const char* message) {

}

char* generate_message_frame(const char* message, size_t* length, bool mask) {
	size_t message_length = strlen(message);
	//FRSV|OP| MASK|PAYLOAD LENGTH
	char *return_message = malloc(message_length + 14);
	return_message[0] = 0b10000001;

	size_t msg_offset = 2;//FIN, RSV, OPCODE, MASK and LENGTH
	if(mask) msg_offset += 4;
	//GET MESSAGE LENGTH
	if(message_length > 125) {
		if(message_length > 65535) {
			return_message[1] = 127;
			*(uint64_t*)(return_message + 2) = message_length;
			msg_offset += 8;
		} else {
			return_message[1] = 126;
			*(uint16_t*)(return_message + 2) = message_length;
			msg_offset += 2;
		}
	} else {
		return_message[1] = message_length;
	}
	//ADD MASK BIT
	return_message[1] |= (mask << 7);

	memcpy(return_message + msg_offset, message, message_length);
	if(mask) {
		uint32_t mask = 0x37fa213d;//TODO: About random - Randomness Requirements for Security RFC4086(https://tools.ietf.org/html/rfc4086)
		reverse_array(&mask, sizeof(mask));

		mask_string(return_message + msg_offset, mask);
		*(uint32_t*)&return_message[msg_offset - 4] = mask;
	}

	*length = message_length + msg_offset;
	return return_message;
}
#endif
