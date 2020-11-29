#include <stdio.h>
#include <time.h>

#include "websocket/websocket.h"

void handle_message(ws_data_t data, websocket_t* websocket);
void open(websocket_t* websocket);

int main(int argc, char *args[]) {
	srand(time(NULL));

	websocket_t websocket;
	memset(&websocket, 0, sizeof(websocket));
	ws_hook_new_message(&websocket, handle_message);
	ws_hook_open(&websocket, open);

	if(ws_connect(&websocket, "echo.websocket.org", 443) == -1) {
		printf("Error occured. Exiting...\n");
		return -1;
	}

	uint8_t buffer[16];
	for(int i = 0; i < 16; i++)
		buffer[i] = rand();
	ws_send_binary(&websocket, buffer, 16);
	while(websocket.connection == CONNECTED) {
		char buffer[64];
		scanf("%s[^\n]", buffer);
		ws_send_message(&websocket, buffer);
	}

	return 0;
}
void open(websocket_t* websocket) {
	printf("Successfuly connected!\n");
}

void handle_message(ws_data_t data, websocket_t* websocket) {
	if(data.type == TEXT_MESSAGE) {
		printf("New message: %s(%ld)\n", data.msg, data.len);
	} else if(data.type == BINARY_MESSAGE){
		printf("New binary data: [");
		for(int i = 0; i < data.len; i++) {
			printf("0x%hhX, ", data.msg[i]);
		}
		printf("]\n");
	}
}
