#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdlib.h>
#include <stdbool.h>

#include "generator.h"

#define HEADER_BUFFER_SIZE 256

typedef struct addrinfo addrinfo_t;
int ws_connect(const char* ip, const char* port) {
	addrinfo_t hint, *res;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, port, &hint, &res);

	int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("Try to connect...\n");
	if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		printf("Error connect to server!\n");
		return -1;
	}
	printf("Success connected!\n");
	//Send handshake request
	char header[256];
	int strlen = sprintf(header, "GET ws://%s:%s/ HTTP/1.1\r\nHost: %s\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: ZMOruJy3LW4qZvJEP55dBg==\r\n\r\n", ip, port, ip);//TODO: Make dynamic WebSocket key generator
	if(send(sockfd, header, strlen, 0) != strlen) {
		printf("Sended bytes not equal own message!\n");
		return -1;
	}
	printf("Success sended!\n");

	//Read hendshake response
	char header_res[HEADER_BUFFER_SIZE];
	memset(header_res, 0, HEADER_BUFFER_SIZE);
	read(sockfd, header_res, HEADER_BUFFER_SIZE);
	printf("Connection output: %s\n", header_res);

	return sockfd;
}

void ws_close(int wsfd, const char* message) {
	//Close the connection
	//0000 1000 1000 0000
	//							  FRSV|OP| MASK|PAYLOAD LENGTH
	uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
	send(wsfd, control_frame, 6, 0);
}

void ws_and_sock_close(int wsfd) {
	ws_close(wsfd, "");
	shutdown(wsfd, SHUT_RDWR);
	close(wsfd);
}

//TODO: Change sockfd to ws struct
void ws_send_message(int wsfd, const char* message) {
	size_t length;
	char* msg = generate_message_frame(message, &length, true);
	printf("Length: %ld\n", length);
	printf("Data of sended message: [");
	for(int i = 0; i < length; i++) {
		printf("0x%X, ", (uint8_t)message[i]);
	}
	printf("]\n");
	send(wsfd, msg, length, 0);
}
#endif
