#include <stdlib.h>
}

//TODO: Change sockfd to ws struct
void ws_send_message(int sockfd, const char* message) {


}

char* generate_message_frame(const char* message, size_t* length) {
	size_t message_length = strlen(message);
	//FRSV|OP| MASK|PAYLOAD LENGTH
	char *return_message = malloc(message_length + 2);
	return_message[0] = 0b10000001;
	if(message_length <= 125) {
		return_message[1] = message_length;
	}
	memcpy(return_message + 2, message, message_length);

	*length = message_length + 2;

	return return_message;
