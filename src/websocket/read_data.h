#ifndef READ_DATA_H
#define READ_DATA_H
#include "websocket.h"
#define MESSAGE_BUFFER_SIZE 132
void read_message(websocket_t* websocket, size_t data_len, uint8_t type) {
	bool is_allocated = false;
	//Mask get
	int wsfd = websocket->fd;
	uint8_t* data;
	if(data_len > 125) {
		size_t need_read;
		uint8_t buffer[8];
		if(data_len == 126) {
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
		data = malloc(need_read + 1);
		while(s_readed < need_read) {
			size_t readed = read(wsfd, data + s_readed, need_read - s_readed);
			s_readed += readed;
			printf("Readed: %ld/%ld\n", s_readed, need_read);
		}
		data_len = need_read;
		is_allocated = true;
	} else {
		data = alloca(data_len + 1);
		read(wsfd, data, data_len);
	}
	data[data_len] = '\0';
	if(websocket->new_message_hook) websocket->new_message_hook((ws_data_t){data, data_len, type}, websocket);
	if(is_allocated)
		free(data);
}
void *read_data(void* params) {
	//Reading frames
	websocket_t* websocket = (websocket_t*)params;
	int wsfd = websocket->fd;
	uint8_t buffer[16];
	while(read(wsfd, buffer, 2) != 0) {
		uint8_t first_byte = buffer[0] - '0';
		size_t msg_len = buffer[1] & 0x7F;
		switch(first_byte & 0xF) {
			case TEXT_MESSAGE:
				read_message(websocket, msg_len, TEXT_MESSAGE);
			break;
			case BINARY_MESSAGE:
				read_message(websocket, msg_len, BINARY_MESSAGE);
			break;
			case CLOSE_CONNECTION:
				if(websocket->close_hook) websocket->close_hook((ws_data_t){}, websocket);
				websocket->connection = DISCONNECTED;
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
	ws_close(websocket, "");
	service_close(websocket);

	return 0;
}
#endif
