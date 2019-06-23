/****************************************************************
* main file transfer program using UDP network protocol
* there is no server/client structure, its a peer-to-peer.
* a program behave either server/client according to the 
* operation it performs such as send or receive file
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "types.h"
#include "data.h"
#include "sender.h"
#include "receiver.h"

//sends response packet type defined in data_packet structure to the given socket
//packet types are defined in data.h file
void send_response(int* socket, struct sockaddr_in* _sockaddr, int type)
{
  struct data_packet respond;
  uint_t sent_data_size;

  respond.type = type;

  sent_data_size = sendto(*socket, &respond, sizeof(struct data_packet), 0, 
                         (struct sockaddr*)&(*_sockaddr), sizeof(struct sockaddr_in));
  if(sent_data_size < 0){
    printf("Error to send response\n");
  }
}

//receives the response type from given socket and returns it
//packet types are defined in data.h file
uint_t receive_response(int* socket, struct sockaddr_in* _sockaddr)
{
  struct data_packet respond;
  uint_t recv_data_size, receiver_addr_len;

  receiver_addr_len = sizeof(struct sockaddr_in);

  recv_data_size = recvfrom(*socket, &respond, sizeof(struct data_packet), 0, 
                            (struct sockaddr*)&(*_sockaddr), &receiver_addr_len);

  if(recv_data_size < 0){
    printf("Failed to receive response from receiver\n");
    return DONE;
  }
	
  return respond.type;
}


int main(int argc, char*argv[])
{
  int _socket;
  struct sockaddr_in sender_sockaddr, receiver_sockaddr;
  uint_t receiver_addr_len;
  char request_buffer[16];
  ushort_t port_num = 1234;
  int bind_result, recv_msg_size;
  const char* ip;
  char *file;

  //check for command args
  if(argc < 2){
    printf("For sender: %s send <filename>\n", argv[0]);
    printf("For receiver: %s receive <ip-address>\n", argv[0]);
    return 0;
  }
  
  //if receive option is passed to command line,
  //then expect next command line option to be the ip address
  //from where to receive the file
  if(strcmp(argv[1], "receive") == 0)
    ip = argv[2];

  //if send option is passed to the command line,
  //then expect next command line option to be the filename
  //to send to the receiver
  if(strcmp(argv[1], "send") == 0){
    file = argv[2];
    printf("File '%s'\n", file);
  }

  //create User Datagram (UDP) socket
  _socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(_socket < 0){
    printf("socket() failed");
    return 0;
  }

  //construct address structure according to the send/receive options
  memset(&sender_sockaddr, 0, sizeof(sender_sockaddr));

  sender_sockaddr.sin_family = AF_INET;
  if(strcmp(argv[1], "receive") == 0)
    sender_sockaddr.sin_addr.s_addr = inet_addr(ip);
  else
    sender_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
  //a default port 1234 is used, but this can be change
  //or can be passed by command line
  sender_sockaddr.sin_port = htons(port_num);

  //bind to the socket to the local address
  bind_result = bind(_socket, (struct sockaddr*)&sender_sockaddr, sizeof(sender_sockaddr));
  //if bind fails, then that socket is already bound to that port
  //that means other peer or end is ready for transmission of data
  //after request is made
  if(bind_result < 0){
    file_receiver(&_socket, &sender_sockaddr);
    return 0;
  }

  receiver_addr_len = sizeof(receiver_sockaddr);

  //receive request from receiver and wait for it to connect
  recv_msg_size = recvfrom(_socket, request_buffer, 16, 0, 
                         (struct sockaddr*)&receiver_sockaddr, &receiver_addr_len);

  if(recv_msg_size < 0){
    printf("recvfrom() failed");
    return 0;
  }

  printf("Connected to %s\n", inet_ntoa(receiver_sockaddr.sin_addr));

  //send the requested file to the receiver
  file_sender(&_socket, &receiver_sockaddr, file);

  return 0;

}

