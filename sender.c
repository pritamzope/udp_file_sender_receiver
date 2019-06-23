/****************************************************************
* UDP file sender program that calculates CRC, attaches it to
* the data being send and sends the data packet with packet type 
* in it and size of the data available on data buffer
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "types.h"
#include "crc.h"
#include "sender.h"
#include "data.h"

//send the filename to the receiver
void send_filename(int* socket, struct sockaddr_in* _sockaddr, char* filename)
{
  struct sockaddr_in receiver = *_sockaddr;
  uint_t receiver_addr_len, sent_data_size, recv_data_size;
  int filename_len = strlen(filename);
  struct data_packet dp;
  uint_t response;

  //continue until sender receiver OK response
  //from receiver
  do{
    dp.type = FILENAME;
    dp.buf_size = filename_len;

    //attach CRC to the each data in the buffer
    for(int i = 0; i < dp.buf_size; i++){
      dp.buffer[i] = filename[i];
      dp.buffer[i] <<= 4;
      uint_t result = get_cyclic_redundancy_check_result(dp.buffer[i], CRC_DIVISOR);
      dp.buffer[i] |= result;
    }

    receiver_addr_len = sizeof(struct sockaddr_in);

    //send it to the receiver
    sent_data_size = sendto(*socket, &dp, sizeof(struct data_packet), 0, 
                            (struct sockaddr*)&receiver, receiver_addr_len);

    if(sent_data_size < 0){
      printf("Failed to send filename to receiver\n");
      return;
    }

    response = receive_response(&(*socket), &(*_sockaddr));

  }while(response != OK);

}

//send a file by reading the given file block by block
//the size of block is defined in data.h file BUFFER_SIZE
void send_file(int* socket, struct sockaddr_in* _sockaddr, char* filename)
{
  struct sockaddr_in receiver = *_sockaddr;
  uint_t receiver_addr_len, sent_data_size, recv_data_size;
  struct data_packet dp;
  byte data[BUFFER_SIZE];
  uint_t read_size, response, total_bytes_sent = 0;

  //open the requested file for reading in binary format
  FILE* fp = fopen(filename, "rb");

  if(fp == NULL){
    printf("Cannot found file %s\n", filename);
    return;
  }

  printf("Sending file, Please wait...\n");

  //continue until DONE response is received
  do{
    //zero out data buffer and packet
    memset(data, 0, BUFFER_SIZE);
    memset((void*)&dp, 0, sizeof(struct data_packet));

    //read block from a file
    read_size = fread(data, sizeof(byte), BUFFER_SIZE - 1, fp);
    if(read_size <= 0){
      send_response(&(*socket), &(*_sockaddr), DONE);
      break;
    }

    dp.type = DATA;
    dp.buf_size = read_size;
    //attach CRC to the data
    for(int i = 0; i < dp.buf_size; i++){
      dp.buffer[i] = data[i];
      dp.buffer[i] <<= 4;
      uint_t result = get_cyclic_redundancy_check_result(dp.buffer[i], CRC_DIVISOR);
      dp.buffer[i] |= result;
    }

    receiver_addr_len = sizeof(struct sockaddr_in);

    //send data packet to the receiver
    sent_data_size = sendto(*socket, &dp, sizeof(struct data_packet), 0, 
                          (struct sockaddr*)&receiver, receiver_addr_len);

    if(sent_data_size < 0){
      printf("Failed to send file data to receiver\n");
      fclose(fp);
      return;
    }

    total_bytes_sent += read_size;

    //receive response from the receiver
    response = receive_response(&(*socket), &(*_sockaddr));

    //keep resending the packet until response is OK
    while(response == RESEND){

      sent_data_size = sendto(*socket, &dp, sizeof(struct data_packet), 0, 
                             (struct sockaddr*)&receiver, receiver_addr_len);

      if(sent_data_size < 0){
        printf("Failed to send file data to receiver\n");
        fclose(fp);
        return;
      }

      response = receive_response(&(*socket), &(*_sockaddr));

    }

  }while(response != DONE);

  printf("Total bytes sent %d, %.4fMB\n", total_bytes_sent, (double)total_bytes_sent/(1024 * 1024));

  fclose(fp);

}

void file_sender(int* socket, struct sockaddr_in* _sockaddr, char* filename)
{
  send_filename(&(*socket), &(*_sockaddr), filename);

  send_file(&(*socket), &(*_sockaddr), filename);	

}

