#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "types.h"
#include "helper.h"

#define HEADER_BUFFER_SIZE 512
#define MESSAGE_BUFFER_SIZE 128

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
	char* message = generate_message_frame("Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE Fuck OFFF DUDE", &length);
	printf("Length: %ld\n", length);
	send(sockfd, message, length, 0);

	//Reading frames
	char buffer[MESSAGE_BUFFER_SIZE];
	while(read(sockfd, buffer, MESSAGE_BUFFER_SIZE) != 0) {
		uint8_t first_byte = buffer[0] - '0';
		int msg_len;
		switch(first_byte & 0xF) {
			case TEXT_MESSAGE:
				msg_len = buffer[1];
				printf("Response(%dc.): %.*s\n", msg_len, msg_len, buffer + 2);
				if(buffer[2] == '6') {
					//Close the connection
					//0000 1000 1000 0000
					//							  FRSV|OP| MASK|PAYLOAD LENGTH
					uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
					send(sockfd, control_frame, 6, 0);
				}
				memset(buffer, 0, msg_len);
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
				printf("Not implemented frame! %s\n", buffer);
			break;
		}
	}

CLOSE_SOCKET:
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);

	return 0;
}
