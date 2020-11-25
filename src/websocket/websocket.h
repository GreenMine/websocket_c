#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "hook.h"
#include "websocket_struct.h"
#include "types.h"
#include "helper.h"

#include "generator.h"

#define HEADER_BUFFER_SIZE 256

void ws_hook_new_message(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*));
void			 ws_hook_close(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*));
void						  ws_hook_open(websocket_t* websocket, void (*f)(websocket_t*));

void *read_data(void* params);

int ws_connect(websocket_t* websocket, const char* ip, const char* port);
void ws_close(websocket_t* websocket, const char* message);
void ws_and_service_close(websocket_t* websocket);
//TODO: Change sockfd to ws struct
void ws_send_message(websocket_t* websocket, const char* message);
void ws_send_binary(websocket_t* websocket, uint8_t* data, size_t len);
#endif
