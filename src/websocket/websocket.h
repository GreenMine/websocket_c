#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "config.h"
#if SSL_CONN==1
#define CONN_TYPE SSL*
#define SEND(conn, buf, buf_len) SSL_write(conn, buf, buf_len)
#define READ(conn, buf, buf_len) SSL_read(conn, buf, buf_len)
#else
#define CONN_TYPE int
#define SEND(sockfd, buf, buf_len) send(sockfd, buf, buf_len, 0)
#define READ(sockfd, buf, buf_len) read(sockfd, buf, buf_len)
#endif

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#if SSL_CONN==1
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#include "hook.h"
#include "websocket_struct.h"
#include "types.h"
#include "helper.h"

#include "generator.h"

#define HEADER_BUFFER_SIZE 1

void ws_hook_new_message(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*));
void 	   ws_hook_close(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*));
void 				   ws_hook_open(websocket_t* websocket, void (*f)(websocket_t*));

void *read_data(void* params);

int ws_connect(websocket_t* websocket, const char* ip, size_t port);

void ws_close(websocket_t* websocket, const char* _message);
void ws_pong(websocket_t* websocket);
void ws_ping(websocket_t* websocket);
void ws_send_message(websocket_t* websocket, const char* message);
void ws_send_binary(websocket_t* websocket, uint8_t* data, size_t len);

void ws_and_service_close(websocket_t* websocket);
void service_close(websocket_t* websocket);
#endif
