#include <stdlib.h>

//TODO: Change sockfd to ws struct
void ws_send_message(int sockfd, const char* message) {

}

char* generate_message_frame(const char* message, size_t* length) {
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

	memcpy(return_message + length_of_size + 2, message, message_length);

	*length = message_length + length_of_size + 2;

	return return_message;
}

void revese_array(char* array, size_t length) {
	for(int i = 0; i < length / 2; i++) {
		char temp = array[i];
		array[i] = array[length-i-1];
		array[length-i-1] = temp;
	}
}
