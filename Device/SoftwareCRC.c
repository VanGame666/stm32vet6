
#include "SoftwareCRC.h"


uint32_t SoftCRC32(uint8_t* data, uint16_t len)
{
	uint32_t CRC32 = 0xFFFFFFFF;

	for(int n = 0; n < len; n++)
	{
		CRC32 = CRC32 ^ (data[n]);

		for(int i = 0; i < 8; i++)  /*此处的8 -- 指每一个char类型有8bit，每bit都要处理*/
		{
			if(CRC32 & 0x00000001)
			{
				CRC32 = CRC32 >> 1;
				CRC32 = CRC32 ^ 0xedb88320;   //多项式0x04c11db7的倒序后是0xedb88320
			}else{
				CRC32 = CRC32 >> 1;
			}
    }
  }
  return CRC32 ^ 0xFFFFFFFF;;
}


uint16_t ModBusCRC16(uint8_t *data, uint16_t len)
{
    unsigned int i, j, tmp;

    uint16_t CRC16 = 0xFFFF;             //CRC寄存器初始值

    for (i = 0; i < len; i++)
    {
        CRC16 = CRC16 ^ (data[i]);

        for (j = 0; j < 8; j++)
        {
            if (CRC16 & 0x0001)
            {
                CRC16 = CRC16 >> 1;
                CRC16 = CRC16 ^ 0xA001;
            }else{
				CRC16 = CRC16 >> 1;
			}
        }
    }

    return CRC16;
}
