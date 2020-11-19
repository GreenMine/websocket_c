#ifndef WEBSOCKET_STRUCT_H
#define WEBSOCKET_STRUCT_H
typedef struct websocket {
	int fd;
	uint8_t connection;
	void (*new_message_hook)(char*);
	void 	   (*close_hook)(char*);
} websocket_t;
#endif
