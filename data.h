#ifndef DATA_H
#define DATA_H

#include "types.h"

//buffer size for data transmission
#define BUFFER_SIZE 512

//default crc divisor
#define CRC_DIVISOR 0b11001

enum packet_type{
  DATA,
  FILENAME,
  OK,
  DONE,
  RESEND
};

struct data_packet
{
  enum packet_type type;
  ushort_t buf_size;
  ushort_t buffer[BUFFER_SIZE];
};


#endif
