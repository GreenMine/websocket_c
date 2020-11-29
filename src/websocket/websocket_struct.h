#ifndef WEBSOCKET_STRUCT_H
#define WEBSOCKET_STRUCT_H
typedef struct websocket {
#ifdef SSL_CONN
	SSL* conn;
#endif
	uint8_t connection;
	pthread_t pthread;
	void (*new_message_hook)(ws_data_t, struct websocket*);
	void	   (*close_hook)(ws_data_t, struct websocket*);
	void				   (*open_hook)(struct websocket*);
} websocket_t;
#endif
