#ifndef SENDER_H
#define SENDER_H

#include "types.h"

extern void send_response(int*, struct sockaddr_in*, int);
extern uint_t receive_response(int*, struct sockaddr_in*);

extern void send_filename(int*, struct sockaddr_in*, char*);
extern void send_file(int*, struct sockaddr_in*, char*);
extern void file_sender(int*, struct sockaddr_in*, char*);

#endif

