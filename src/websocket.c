#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "hook.h"
#include "websocket_struct.h"
#include "types.h"
#include "helper.h"

#include "websocket.h"
#include "read_loop.h"

void handle_message(ws_data_t data, websocket_t* websocket);
void open(websocket_t* websocket);

int main(int argc, char *args[]) {

	websocket_t websocket;
	memset(&websocket, 0, sizeof(websocket));
	ws_hook_new_message(&websocket, handle_message);
	ws_hook_open(&websocket, open);

	if(ws_connect(&websocket, "localhost", "80") == -1) {
		printf("Error occured. Exiting...\n");
		return -1;
	}

	while(websocket.connection == CONNECTED) {
		char buffer[64];
		scanf("%s[^\n]", buffer);
		ws_send_message(&websocket, buffer);
	}

	printf("End...\n");
	return 0;
}
void open(websocket_t* websocket) {
	printf("Successfuly connected!\n");
}

void handle_message(ws_data_t data, websocket_t* websocket) {
	printf("New message: %s(%ld)\n", data.msg, data.len);
}
