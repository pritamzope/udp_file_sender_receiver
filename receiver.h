#ifndef RECEIVER_H
#define RECEIVER_H

#include "types.h"

extern void send_response(int*, struct sockaddr_in*, int);
extern uint_t receive_response(int*, struct sockaddr_in*);

extern void receive_filename(int*, struct sockaddr_in*, char*);
extern void receive_file(int*, struct sockaddr_in*, char*);
extern void file_receiver(int*, struct sockaddr_in*);

#endif
