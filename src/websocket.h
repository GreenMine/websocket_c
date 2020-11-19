#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdlib.h>
#include <stdbool.h>

#include "generator.h"

#define HEADER_BUFFER_SIZE 256

void *read_data(void* params);

int ws_connect(websocket_t* websocket, const char* ip, const char* port) {
	struct addrinfo hint, *res;
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

	websocket->fd = sockfd;
	websocket->connection = CONNECTED;

	//Thread to read messages
	pthread_t tid;
	pthread_create(&tid, NULL, read_data, websocket);

	return 0;
}

void ws_hook_event(websocket_t* websocket, uint8_t event, void (*f)(char*)) {
	switch(event) {
		case NEW_MESSAGE:
			websocket->new_message_hook = f;
			break;
		case CLOSE:
			websocket->close_hook = f;
			break;
	}
}

void ws_close(websocket_t* websocket, const char* message) {
	//Close the connection
	//0000 1000 1000 0000
	//							  FRSV|OP| MASK|PAYLOAD LENGTH
	uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
	send(websocket->fd, control_frame, 6, 0);
	websocket->connection = DISCONNECTED;
}

void ws_and_sock_close(websocket_t* websocket) {
	int wsfd = websocket->fd;
	ws_close(websocket, "");
	shutdown(wsfd, SHUT_RDWR);
	close(wsfd);
}

//TODO: Change sockfd to ws struct
void ws_send_message(websocket_t* websocket, const char* message) {
	size_t length;

	uint8_t* msg = generate_data_frame(TEXT_MESSAGE, (uint8_t*)message, strlen(message), &length, true);
//	printf("Length: %ld\n", length);
//	printf("Data of sended message: [");
//	for(int i = 0; i < length; i++) {
//		printf("0x%X, ", (uint8_t)msg[i]);
//	}
//	printf("]\n");
	send(websocket->fd, msg, length, 0);

	free(msg);
}

void ws_send_binary(websocket_t* websocket, uint8_t* data, size_t len) {
	size_t length;

	reverse_array(data, len);
	uint8_t* send_data = generate_data_frame(BINARY_MESSAGE, data, len, &length, true);
	send(websocket->fd, send_data, length, 0);

	free(send_data);
}
#endif
