/****************************************************************
* UDP file receiver program that calculates CRC for error detection
* from each data packet buffer, if any error is found then 
* sends RESEND response to the sender otherwise writes that data to a file
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "types.h"
#include "crc.h"
#include "data.h"
#include "receiver.h"

//receive filename from sender
void receive_filename(int* socket, struct sockaddr_in* _sockaddr, char* filename)
{
  struct sockaddr_in sender = *_sockaddr;
  uint_t sender_addr_len, sent_data_size, recv_data_size;
  int filename_len = strlen(filename);
  struct data_packet dp;
  bool crc_error = false;
  byte data[BUFFER_SIZE];

  sender_addr_len = sizeof(struct sockaddr_in);

  //continue until there is no CRC error
  do{
    crc_error = false;

    //read filename data packet
    recv_data_size = recvfrom(*socket, &dp, sizeof(struct data_packet), 0, 
                             (struct sockaddr*)&sender, &sender_addr_len);

    if(recv_data_size < 0){
      printf("Failed to receive filename from sender\n");
      return;
    }

    //check for error in each data packet buffer item
    for(int i = 0; i < dp.buf_size; i++){
      if(cyclic_redundancy_check(dp.buffer[i], CRC_DIVISOR) == false){
        printf("CRC check error, Resend\n");
        crc_error = true;
        send_response(&(*socket), &(*_sockaddr), RESEND);
      }
      dp.buffer[i] >>= 4;
    }

  }while(crc_error == true);

  //send OK response to the sender for sending file contents
  send_response(&(*socket), &(*_sockaddr), OK);

  //get filaname
  for(int i = 0; i < dp.buf_size; i++)
    data[i] = (byte)dp.buffer[i];

  strncpy(filename, data, dp.buf_size);

}

//receive file 
void receive_file(int* socket, struct sockaddr_in* _sockaddr, char* filename)
{
  struct sockaddr_in sender = *_sockaddr;
  uint_t sender_addr_len, sent_data_size, recv_data_size;
  struct data_packet dp;
  byte data[BUFFER_SIZE];
  bool crc_error = false;
  uint_t total_bytes_received = 0;

  //create/open a file received from sender
  FILE* fp = fopen(filename, "wb+");

  if(fp == NULL){
    printf("Unable to create file %s\n", filename);
    return;
  }

  printf("Receiving file, Please wait...\n");

  sender_addr_len = sizeof(struct sockaddr_in);

  //continue until DONE response is received
  do{
    //zero out data buffer and data packet
    memset(data, 0, BUFFER_SIZE);
    memset((void*)&dp, 0, sizeof(struct data_packet));

    crc_error = false;

    //receive file content data packet
    recv_data_size = recvfrom(*socket, &dp, sizeof(struct data_packet), 0, 
                             (struct sockaddr*)&sender, &sender_addr_len);

    if(recv_data_size < 0){
      printf("Unable to receive file data\n");
      fclose(fp);
      return;
    }
    if(dp.buf_size == 0){
      send_response(&(*socket), &(*_sockaddr), DONE);
      printf("Total bytes received %d,  %.4fMB\n", 
              total_bytes_received, (double)total_bytes_received/(1024 * 1024));
      fclose(fp);
      return;
    }

    total_bytes_received += dp.buf_size;

    //check for CRC error in each data packet buffer item
    if(dp.type == DATA){
      for(int i = 0; i < dp.buf_size; i++){
        if(cyclic_redundancy_check(dp.buffer[i], CRC_DIVISOR) == false){
          printf("CRC check error, Resend\n");
          crc_error = true;
          break;
        }

        dp.buffer[i] >>= 4;

        if(crc_error) send_response(&(*socket), &(*_sockaddr), RESEND);
      }

      for(int i = 0; i < dp.buf_size; i++)
        data[i] = (byte)dp.buffer[i];
    }

    //send OK response to the sender for sending other packets
    send_response(&(*socket), &(*_sockaddr), OK);

    //write received data to a file
    fwrite(data, sizeof(byte), dp.buf_size, fp);

  }while(dp.type != DONE);

  //send DONE response to the sender as completion
  send_response(&(*socket), &(*_sockaddr), DONE);

  printf("Total bytes received %d,  %dMB\n", 
              total_bytes_received, total_bytes_received/(1024 * 1024));

  fclose(fp);
	
}

//request the sender for connection
void file_receiver(int* socket, struct sockaddr_in* _sockaddr)
{
  struct sockaddr_in sender = *_sockaddr;
  uint_t sender_addr_len, sent_data_size;
  char str[16] = "Hello";
  byte filename[BUFFER_SIZE];

  sender_addr_len = sizeof(struct sockaddr_in);

  //send some data to the sender so that sender can
  //initiate the connection, here i'm sending 'Hello' string
  sent_data_size = sendto(*socket, str, 16, 0,
                         (struct sockaddr*)&(*_sockaddr), sender_addr_len);

  if(sent_data_size != 16){
    printf("Error to establish connection with sender %d\n", sent_data_size);
    return;
  }

  //receive the filename from sender
  receive_filename(&(*socket), &(*_sockaddr), filename);

  printf("Received filename: '%s'\n", filename);

  //receive the data of the file
  receive_file(&(*socket), &(*_sockaddr), filename);

}


