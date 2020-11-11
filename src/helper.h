#include <stdlib.h>
#include <stdbool.h>

//TODO: Change sockfd to ws struct
void ws_send_message(int sockfd, const char* message) {

}

char* generate_message_frame(const char* message, size_t* length, bool mask) {
	size_t message_length = strlen(message);
	//FRSV|OP| MASK|PAYLOAD LENGTH
	char *return_message = malloc(message_length + 10);
	return_message[0] = 0b10000001;
	size_t length_of_size = 0;
	if(message_length > 125) {
		if(message_length > 65535) {
			return_message[1] = 127;
			*(uint64_t*)(return_message + 2) = message_length;
			length_of_size = 8;
		} else {
			return_message[1] = 126;
			*(uint16_t*)(return_message + 2) = message_length;
			length_of_size = 2;
		}
	}
	return_message[1] |= (mask << 7);

	memcpy(return_message + length_of_size + 2, message, message_length);
	*length = message_length + length_of_size + 2;

	return return_message;
}

void reverse_array(void* array, size_t length) {
	uint8_t* ptr = (uint8_t*)array;
	for(int i = 0; i < length / 2; i++) {
		uint8_t temp = ptr[i];
		ptr[i] = ptr[length-i-1];
		ptr[length-i-1] = temp;
	}
}
