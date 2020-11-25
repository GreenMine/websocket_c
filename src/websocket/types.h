#ifndef TYPES_H
#define TYPES_H

//FRAMES TYPES
#define CONTINUATION 	 0x0
#define TEXT_MESSAGE 	 0x1
#define BINARY_MESSAGE 	 0x2
#define CLOSE_CONNECTION 0x8
#define PING 			 0x9
#define PONG 			 0xA

//CONNECTION TYPES
#define CONNECTED		 0x1
#define DISCONNECTED	 0x2

//EVENT TYPES
#define NEW_MESSAGE 	 0x1
#define CLOSE			 0x2
#define CONNECT			 0x3

#endif
