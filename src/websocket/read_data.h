#ifndef READ_DATA_H
#define READ_DATA_H
#include "websocket.h"
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
				if(websocket->new_message_hook) websocket->new_message_hook((ws_data_t){msg, msg_len, TEXT_MESSAGE}, websocket);
				if(is_allocated)
					free(msg);
			break;
			case BINARY_MESSAGE:
				printf("Umimplemented binary data!\n");
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
