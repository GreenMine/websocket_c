#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "types.h"
#include "helper.h"
#include "mask.h"

#include "websocket.h"

#define HEADER_BUFFER_SIZE 512
#define MESSAGE_BUFFER_SIZE 132

typedef struct addrinfo addrinfo_t;
int main(int argc, char *args[]) {

	addrinfo_t hint, *res;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	getaddrinfo("localhost", "80", &hint, &res);

	int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("Try to connect...\n");
	if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		printf("Error connect to server!\n");
		return -1;
	}
	printf("Success connected!\n");
	//Send handshake request
const char* header = "GET ws://localhost/ HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: ZMOruJy3LW4qZvJEP55dBg==\r\n\r\n";
	if(send(sockfd, header, strlen(header), 0) != strlen(header)) {
		printf("Sended bytes not equal own message!\n");
		return -1;
	}
	printf("Success sended!\n");

	//Read hendshake response
	char header_res[HEADER_BUFFER_SIZE];
	memset(header_res, 0, HEADER_BUFFER_SIZE);
	read(sockfd, header_res, HEADER_BUFFER_SIZE);
	printf("Connection output: %s\n", header_res);

	//Close the connection
	//0000 1000 1000 0000
	//					FRSV|OP| MASK|PAYLOAD LENGTH
	size_t length;
	char* message = generate_message_frame("Something", &length, true);
	printf("Length: %ld\n", length);
	printf("Data: [");
	for(int i = 0; i < length; i++) {
		printf("0x%X, ", (uint8_t)message[i]);
	}
	printf("]\n");
	send(sockfd, message, length, 0);

	//Reading frames
	char *buffer = alloca(MESSAGE_BUFFER_SIZE);
	while(read(sockfd, buffer, MESSAGE_BUFFER_SIZE) != 0) {
		uint8_t first_byte = buffer[0] - '0';
		size_t msg_len;
		switch(first_byte & 0xF) {
			case TEXT_MESSAGE:
				msg_len = buffer[1] & 0x7F;
				size_t msg_offset = 2;

				if(buffer[1] & 0x80) msg_offset += 4;
				if(msg_len > 125) {
					size_t need_read;
					if(msg_len == 126) {
						reverse_array(buffer + 2, 2);
						need_read = *(uint16_t*)(buffer + 2);
						msg_offset += 2;
					} else {
						reverse_array(buffer + 2, 8);
						need_read = *(uint64_t*)(buffer + 2);
						msg_offset += 8;
					}

					size_t s_readed = MESSAGE_BUFFER_SIZE - msg_offset;
					char *big_msg = malloc(MESSAGE_BUFFER_SIZE + need_read);
					memcpy(big_msg, buffer, MESSAGE_BUFFER_SIZE);
					while(s_readed < need_read) {
						size_t readed = read(sockfd, big_msg + s_readed, need_read - s_readed);
						printf("Readed %ld/%ld, must readed: %ld, but %ld\n", s_readed, need_read, need_read - s_readed, readed);
						s_readed += readed;
					}
					buffer = big_msg;
					msg_len = need_read;
				}

				printf("Response(%dc.): %.*s\n", msg_len, msg_len, buffer + msg_offset);
//					//Close the connection
//					//0000 1000 1000 0000
//					//							  FRSV|OP| MASK|PAYLOAD LENGTH
//					uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
//					send(sockfd, control_frame, 6, 0);
			break;
			case BINARY_MESSAGE:
				printf("Umimplemented binary data!\n");
			break;
			case CLOSE_CONNECTION:
				printf("Server closed the connection!\n");
				goto CLOSE_SOCKET;
			break;
			case PING:
				printf("Ping from server. Respond...\n");
				uint8_t control_frame[6] = {0b10001010, 0b10000000, 0, 0, 0, 0};
				send(sockfd, control_frame, 6, 0);
			break;
			case PONG:
				printf("Pong!\n");
			break;
			default:
				printf("Not implemented frame! %s.\n", buffer);
			break;
		}
	}

CLOSE_SOCKET:
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return 0;
}
