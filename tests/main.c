#include <stdio.h>
#include <websocket.h>

void on_new_message(ws_data_t data, websocket_t* connection);

int main() {

	websocket_t websocket;
	memset(&websocket, 0, sizeof(websocket_t));

	ws_hook_new_message(&websocket, on_new_message);

	ws_connect(&websocket, "echo.websocket.org", 443);

	ws_send_message(&websocket, "test message!");
	while(true);

	return 0;
}

void on_new_message(ws_data_t data, websocket_t* connection) {
	printf("New message: %s\n", data.msg);

}
