#include "websocket.h"

void ws_hook_new_message(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*)) {websocket->new_message_hook = f;}
void			 ws_hook_close(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*)) {websocket->close_hook = f;}
void						  ws_hook_open(websocket_t* websocket, void (*f)(websocket_t*)) {websocket->open_hook = f;}

#define MESSAGE_BUFFER_SIZE 132
void *read_data(void* params) {
	//Reading frames
	websocket_t* websocket = (websocket_t*)params;
	int wsfd = websocket->fd;
	uint8_t buffer[16];
	while(read(wsfd, buffer, 2) != 0) {
		uint8_t first_byte = buffer[0] - '0';
		size_t msg_len = buffer[1] & 0x7F;
		char* msg;
		bool is_allocated = false;
		switch(first_byte & 0xF) {
			case TEXT_MESSAGE:
				//Mask get
				if(msg_len > 125) {
					size_t need_read;
					if(msg_len == 126) {
						read(wsfd, buffer, 2);
						reverse_array(buffer, 2);
						need_read = *(uint16_t*)buffer;
					} else {
						read(wsfd, buffer, 8);
						reverse_array(buffer, 8);
						need_read = *(uint64_t*)buffer;
					}

					printf("Need read: %ld\n", need_read);
					size_t s_readed = 0;
					msg = malloc(need_read + 1);
					while(s_readed < need_read) {
						size_t readed = read(wsfd, msg + s_readed, need_read - s_readed);
						s_readed += readed;
						printf("Readed: %ld/%ld\n", s_readed, need_read);
					}
					msg_len = need_read;
					is_allocated = true;
				} else {
					msg = alloca(msg_len + 1);
					read(wsfd, msg, msg_len);
				}
				msg[msg_len] = '\0';
				websocket->new_message_hook((ws_data_t){msg_len, msg, TEXT_MESSAGE}, websocket);
				if(is_allocated)
					free(msg);
			break;
			case BINARY_MESSAGE:
				printf("Umimplemented binary data!\n");
			break;
			case CLOSE_CONNECTION:
				printf("Server closed the connection!\n");
				websocket->close_hook((ws_data_t){}, websocket);
				goto CLOSE_SOCKET;
			break;
			case PING:
				printf("Ping from server. Respond...\n");
				uint8_t control_frame[6] = {0b10001010, 0b10000000, 0, 0, 0, 0};
				send(wsfd, control_frame, 6, 0);
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
	ws_and_service_close(websocket);

	return 0;
}

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

	if(websocket->open_hook != NULL) websocket->open_hook(websocket);

	websocket->fd = sockfd;
	websocket->connection = CONNECTED;

	//Thread to read messages
	pthread_create(&websocket->pthread, NULL, read_data, websocket);

	return 0;
}

void ws_close(websocket_t* websocket, const char* message) {
	//Close the connection
	//0000 1000 1000 0000
	//							  FRSV|OP| MASK|PAYLOAD LENGTH
	uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
	send(websocket->fd, control_frame, 6, 0);
	websocket->connection = DISCONNECTED;
}

void ws_and_service_close(websocket_t* websocket) {
	int wsfd = websocket->fd;
	ws_close(websocket, "");

	//Service close
	shutdown(wsfd, SHUT_RDWR);
	close(wsfd);

	pthread_cancel(websocket->pthread);
}

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
