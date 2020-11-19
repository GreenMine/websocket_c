#ifndef READ_LOOP_H
#define READ_LOOP_H

#include <stdlib.h>

#define MESSAGE_BUFFER_SIZE 132
void *read_data(void* params) {
	//Reading frames
	websocket_t connection = *(websocket_t*)params;
	int wsfd = connection.fd;
	char *buffer = alloca(MESSAGE_BUFFER_SIZE);
	while(read(wsfd, buffer, MESSAGE_BUFFER_SIZE) != 0) {
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

					printf("Need read: %ld, offset: %ld\n", need_read, msg_offset);
					size_t n = need_read + msg_offset;
					char *big_msg = malloc(n);
					memcpy(big_msg, buffer, MESSAGE_BUFFER_SIZE);
					size_t s_readed = MESSAGE_BUFFER_SIZE;
					while(s_readed < n) {
						size_t readed = read(wsfd, big_msg + s_readed, n - s_readed);
						s_readed += readed;
						printf("Readed: %ld/%ld\n", s_readed, n);
					}
					buffer = big_msg;
					msg_len = need_read;
				}

				connection.new_message_hook(buffer + msg_offset);
//				printf("Response(%ldc.): %.*s\n", msg_len, msg_len, buffer + msg_offset);
//				printf("Response: [");
//				for(int i = 0; i < msg_len + msg_offset; i++)
//					printf("0x%X, ", (uint8_t)buffer[i]);
//				printf("]\n");
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
	ws_and_sock_close(&connection);

	return 0;
}
#endif
