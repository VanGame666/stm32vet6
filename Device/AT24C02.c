#include "AT24C02.h"


static uint8_t AT24CXX_ReadBuff[AT24CXX_BUFF_SIZE];
static uint8_t AT24CXX_WriteBuff[AT24CXX_BUFF_SIZE];


struct StateInfo loginfo;
struct StateInfo loginfo_a;






void struct_decode(void)
{
	struct StateInfo *pStates = (struct StateInfo*)AT24CXX_ReadBuff;

	loginfo_a.time = pStates->time;
	loginfo_a.data = pStates->data;
	loginfo_a.frequency = pStates->frequency;
	loginfo_a.voltage = pStates->voltage;
	loginfo_a.current = pStates->current;
}

void time_decode(void)
{
	uint8_t time_hour =  (loginfo_a.time & 0xFF00) >> 8;
	uint8_t time_min =  (loginfo_a.time & 0x00ff);
	printf("now time:hour %d min: %d\r\n",time_hour,time_min);
}


void data_decode(void)
{
	uint8_t data_year =  (loginfo_a.data & 0xFE00) >> 9;
	uint8_t data_month =  (loginfo_a.data & 0x01E0) >> 5;
	uint8_t data_day =  (loginfo_a.data & 0x001F);
	printf("now data:year %d month: %d day: %d\r\n",data_year+1980,data_month,data_day);
}

uint16_t time_code(uint8_t hour,uint8_t min)
{
	uint16_t time_hour =  (uint16_t)hour << 8;
	uint16_t time_min =  (uint16_t)min;
	uint16_t time_code =  time_hour|time_min;
	printf("now time code :%4x \r\n",time_code);
	return time_code;
}

uint16_t data_code(uint8_t year,uint8_t month,uint8_t day)
{
	year = year -1980;
	uint16_t data_year =  ((uint16_t)year << 9);
	uint16_t data_month =  (uint16_t)month << 5;
	uint16_t data_day =  (uint16_t)day & 0x001F;
	uint16_t data_code = data_year|data_month|data_day;
	printf("now data code :%4x \r\n",data_code);
	return data_code;
}


void AT24TwiceWrite(int pos,struct StateInfo structsend,int index)
{
	HAL_I2C_Mem_Write(&hi2c1, AT24CXX_Write_ADDR,pos,I2C_MEMADD_SIZE_8BIT, (uint8_t*)&structsend+index,1,1000);
	HAL_Delay(10);
	HAL_I2C_Mem_Write(&hi2c1, AT24CXX_Write_ADDR,128+pos,I2C_MEMADD_SIZE_8BIT, (uint8_t*)&structsend+index,1,1000);
	HAL_Delay(10);
}




void write_fault_record(FaultRecord record)
{
    uint8_t next_end = FaultHeader.end_addr + sizeof(FaultRecord);
    if (next_end >= FAULT_STORAGE_END) { // 环形回绕
        next_end = FAULT_STORAGE_START;
    }

	  for(int j = 0;j<sizeof(record);j++)
	  {
	    	AT24TwiceWrite(FaultRecord_ADD + next_end,record,j);
	  }



    // 3. 检查缓冲区是否满
    if (header.fault_count * sizeof(FaultRecord) > FAULT_STORAGE_SIZE) {
        header.start_addr += sizeof(FaultRecord);
        if (header.start_addr >= FAULT_STORAGE_END) {
            header.start_addr = FAULT_STORAGE_START;
        }
    }

    // 4. 写入故障记录到A区
    eeprom_write(header.end_addr, &record, sizeof(FaultRecord));
    header.end_addr = next_end;

    // 5. 更新统计信息头并备份到B区
    update_header_crc(&header);
    eeprom_write(A_HEADER_START, &header, sizeof(FaultHeader));
    backup_to_b_zone();
}
































void AT24C02_Func(void)
{
	loginfo.time = time_code(12,31);
	loginfo.data = data_code(2025,2,26);
	loginfo.frequency = 0x3421;
	loginfo.voltage = 0x2222;
	loginfo.current = 0x2222;
	loginfo.crcheck = Soft_CRC_Calculate(&loginfo,sizeof(loginfo)-4);

	printf("crc %8x\r\n",loginfo.crcheck);

    for(int j = 0;j<sizeof(loginfo);j++)
    {
    	AT24TwiceWrite(j,loginfo,j);
    }

//    HAL_I2C_Mem_Write(&hi2c1, AT24CXX_Write_ADDR,128+9,I2C_MEMADD_SIZE_8BIT, (uint8_t*)&loginfo+1,1,1000);


    HAL_I2C_Mem_Read(&hi2c1, AT24CXX_Read_ADDR,0,I2C_MEMADD_SIZE_8BIT, (uint8_t*)AT24CXX_ReadBuff,sizeof(loginfo),0xFF);
    for(int idx = 0; idx < sizeof(loginfo); idx++) {
    	printf("Struct Maininfo = %2x \r\n",AT24CXX_ReadBuff[idx]);
    }
    if(Soft_CRC_Calculate(AT24CXX_ReadBuff,sizeof(loginfo)-4) == *(uint32_t*)&AT24CXX_ReadBuff[sizeof(loginfo)-4])
    {
    	printf("crc main check success\r\n");
    }
	printf("crc calc %8x\r\n",Soft_CRC_Calculate(AT24CXX_ReadBuff,sizeof(loginfo)-4));
	printf("crc read %8x\r\n",*(uint32_t*)&AT24CXX_ReadBuff[sizeof(loginfo)-4]);


    HAL_I2C_Mem_Read(&hi2c1, AT24CXX_Read_ADDR,128,I2C_MEMADD_SIZE_8BIT, (uint8_t*)AT24CXX_ReadBuff+128,sizeof(loginfo),0xFF);
    for(int idx = 0; idx < sizeof(loginfo); idx++) {
    	printf("Struct Subinfo = %2x \r\n",AT24CXX_ReadBuff[128+idx]);
    }
    if(Soft_CRC_Calculate(&AT24CXX_ReadBuff[128],sizeof(loginfo)-4) == *(uint32_t*)&AT24CXX_ReadBuff[128+sizeof(loginfo)-4])
    {
    	printf("crc sub check success\r\n");
    }
	printf("crc calc %8x\r\n",Soft_CRC_Calculate(AT24CXX_ReadBuff+128,sizeof(loginfo)-4));
	printf("crc read %8x\r\n",*(uint32_t*)&AT24CXX_ReadBuff[128+sizeof(loginfo)-4]);


    struct_decode();
    time_decode();
    data_decode();

}














