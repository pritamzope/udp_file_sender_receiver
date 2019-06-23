/****************************************************************
* Cyclic Redundancy Check error dection
* the divisor polynomial is fixed in data.h file
* polynomial : x^4 + x^3 + 1 => 11001
****************************************************************/

#include "crc.h"
#include "types.h"

//calculate power of 2 until less than the given number
uint_t get_powerof2_min(uint_t num)
{
  uint_t pow2min = 0;
  uint_t pow = 0;
  if(num <= 0) return 0;
  do{
    pow2min = 2 << pow;
    pow++;
  }while(pow2min < num);
  
  return pow - 1;
}

//return remainder CRC result 
uint_t get_cyclic_redundancy_check_result(uint_t num, uint_t divisor)
{
  uint_t num_min_powerof_2, divisor_min_powerof_2;
  uint_t max_divisor = divisor;

  //loop until negative/maximum number is hit
  while(num < 0x7fffffff){
    num_min_powerof_2 = get_powerof2_min(num);
    divisor_min_powerof_2 = get_powerof2_min(divisor);
    //left shift the crc divisor so xoring divisior with number
    //from most significant bit(MSB) side
    max_divisor = divisor << (num_min_powerof_2 - divisor_min_powerof_2);
    //get XOR 
    num = num ^ max_divisor;
    if(num <= 15) break;
    if(max_divisor < 0) break;
  }

  if(num > 0x7fffffff)  num = 0;

  return num;
}

bool cyclic_redundancy_check(uint_t num, uint_t divisor)
{
  if(get_cyclic_redundancy_check_result(num, divisor) == 0)
    return true;

  return false;
}


