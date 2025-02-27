
#include "SoftwareCRC.h"


uint32_t Soft_CRC_Calculate(uint8_t* data, uint16_t len)
{
	uint32_t crc = 0xFFFFFFFF;

	for(int n = 0; n < len; n++)
	{
		crc = (data[n]) ^ crc;

		for(int i = 0; i < 8; i++)  /*此处的8 -- 指每一个char类型有8bit，每bit都要处理*/
		{
			if(crc & 0x01)
			{
				crc = crc >> 1;
				crc = crc ^ 0xedb88320;   //多项式0x04c11db7的倒序后是0xedb88320
			}
			else{
				crc = crc >> 1;
			}
    }
  }
  return crc ^ 0xFFFFFFFF;;
}
