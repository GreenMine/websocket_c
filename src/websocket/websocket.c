#include "websocket.h"
#include "read_data.h"

void ws_hook_new_message(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*)) {websocket->new_message_hook = f;}
void			 ws_hook_close(websocket_t* websocket, void (*f)(ws_data_t, websocket_t*)) {websocket->close_hook = f;}
void						  ws_hook_open(websocket_t* websocket, void (*f)(websocket_t*)) {websocket->open_hook = f;}

int ws_connect(websocket_t* websocket, const char* ip, size_t p) {
	struct addrinfo hint, *res;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;

	char port[16];
	sprintf(port, "%ld", p);

	getaddrinfo(ip, port, &hint, &res);

	if(res == NULL) return -1;
	int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("Try to connect...\n");
	if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		printf("Error connect to server!\n");
		return -1;
	}
	printf("Success connected!\n");

#ifdef SSL_CONN
	printf("Create SSL connection\n");
	SSL_load_error_strings ();
	SSL_library_init ();
	SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());

	SSL *conn = SSL_new(ssl_ctx);
	SSL_set_fd(conn, sockfd);

	int err = SSL_connect(conn);
	if (err != 1)
		abort(); // handle error
	printf("SSL connection success created!\n");
#else
	int conn = sockfd;
#endif

	printf("Length of defined type: %ld\n", sizeof(CONN_TYPE));
	//Send handshake request
	char header[256];
	int strlen = sprintf(header, "GET wss://%s:%s/ HTTP/1.1\r\nHost: %s\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: ZMOruJy3LW4qZvJEP55dBg==\r\n\r\n", ip, port, ip);//TODO: Make dynamic WebSocket key generator
	if(SEND(conn, header, strlen) != strlen) {
		printf("Sended bytes not equal own message!\n");
		return -1;
	}
	printf("Success sended!\n");

	//Read hendshake response
	char header_res = 0;
	int concurrency = 0;
	const char* expected = "\r\n\r\n";
	do {
		READ(conn, &header_res, 1);
		putchar(header_res);
		if(header_res == expected[concurrency]) {
			concurrency++;
		} else {
			concurrency = 0;
		}
	} while(concurrency < 4);

	if(websocket->open_hook) websocket->open_hook(websocket);

	websocket->conn = conn;
	websocket->connection = CONNECTED;

	//Thread to read messages
	pthread_create(&websocket->pthread, NULL, read_data, websocket);

	return 0;
}

void ws_close(websocket_t* websocket, const char* message) {
	//Close the connection
	//0000 1000 1000 0000
	//							  FRSV|OP| MASK|PAYLOAD LENGTH
	uint8_t control_frame[6] = {0b10001000, 0b10000000, 0, 0, 0, 0};
	SEND(websocket->conn, control_frame, 6);
}

void service_close(websocket_t* websocket) {
	CONN_TYPE conn = websocket->conn;
	websocket->connection = DISCONNECTED;
	if(websocket->close_hook) {
		char* msg = "Closed from code!";
		websocket->close_hook((ws_data_t){msg, strlen(msg), CLOSE_CONNECTION}, websocket);
	}

	//Service close
#ifdef SSL_CONN
	SSL_shutdown(conn);
	SSL_free(conn);
	//FIXME: Close socket!
#else
	shutdown(conn, SHUT_RDWR);
	close(conn);
#endif

	pthread_cancel(websocket->pthread);

}

void ws_send_message(websocket_t* websocket, const char* message) {
	size_t length;

	uint8_t* msg = generate_data_frame(TEXT_MESSAGE, (uint8_t*)message, strlen(message), &length, true);
//	printf("Length: %ld\n", length);
//	printf("Data of sended message: [");
//	for(int i = 0; i < length; i++) {
//		printf("0x%X, ", (uint8_t)msg[i]);
//	}
//	printf("]\n");
	SEND(websocket->conn, msg, length);

	free(msg);
}

void ws_send_binary(websocket_t* websocket, uint8_t* data, size_t len) {
	size_t length;

	reverse_array(data, len);
	uint8_t* send_data = generate_data_frame(BINARY_MESSAGE, data, len, &length, true);
	printf("Sended from ws_send_binary: [");
	for(int i = 0; i < length; i++) {
		printf("0x%X, ", send_data[i]);
	}
	printf("]\n");
	SEND(websocket->conn, send_data, length);

	free(send_data);
}
