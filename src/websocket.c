#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>


#include "websocket_struct.h"
#include "types.h"
#include "helper.h"

#include "websocket.h"
#include "read_loop.h"

void handle_message(char* msg);

int main(int argc, char *args[]) {

	websocket_t connection;

	if(ws_connect(&connection, "localhost", "80") == -1) {
		printf("Error occured. Exiting...\n");
		return -1;
	}
	//ws_send_message(wsfd, "Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum Lorem ipsum ");

	ws_hook_event(&connection, NEW_MESSAGE, handle_message);

	while(true) {
		printf("Hey!\n");
		usleep(500000);
	}

	return 0;
}

void handle_message(char* msg) {
	printf("New message: %s\n", msg);
}

