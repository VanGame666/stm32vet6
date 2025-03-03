#include "Protocolprocessing.h"

uint8_t rx_buffer[BUF_SIZE]={0};
uint8_t tx_buffer[BUF_SIZE]={0};

uint8_t rx_num=0;
uint8_t tx_num=0;

uint8_t dacai_head[] = {0xEE};
uint8_t dacai_tail[] = {0xFF,0xFC,0xFF,0xFF};

uint8_t pc_head[] = {0x5A,0xA5};
uint8_t pc_tail[] = {0xA5,0x5A};

PC_Conect_t pc_connect;





#define FRAME_SEND(head,tail) frame_send(head,tail,sizeof(head),sizeof(tail))
void frame_send(uint8_t* head,uint8_t* tail,uint8_t head_size,uint8_t tail_size)
{
	uint8_t frame_size = head_size + tail_size + tx_num;
	uint8_t frame[frame_size];
	uint8_t i,j = 0;
	for(i = 0;i < head_size;i++,j++)
	{
		frame[j] = head[i];
	}
	for(i = 0;i < tx_num;i++,j++)
	{
		frame[j] = tx_buffer[i];
	}
	for(i = 0;i < tail_size;i++,j++)
	{
		frame[j] = tail[i];
	}
	HAL_UART_Transmit(&huart6,&frame,frame_size, 0xFFFF);
}




# define HEAD_VERIFICATION(head) head_verification(head,sizeof(head))
int head_verification(uint8_t* head,uint8_t head_size)
{
	for(int i = 0;i < head_size;i++)
	{
		if(rx_buffer[i] != head[i]){return 0;}
	}
	return 1;
}



# define TIAL_VERIFICATION(tail) tail_verification(tail,sizeof(tail))
int tail_verification(uint8_t* tail,uint8_t tail_size)
{
	for(int i = 0;i < tail_size;i++)
	{
		if(rx_buffer[rx_num-tail_size+i] != tail[i]){return 0;}
	}
	return 1;
}



void CMD_ReadScreen(void)
{
	uint16_t cmd,param,crc16;
	tx_num = sizeof(cmd)+sizeof(crc16);

	cmd  = CharReverse16(0xB101);
	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));

	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
	crc16 = CharReverse16(crc16);
	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));

	FRAME_SEND(dacai_head,dacai_tail);
}



void CMD_SwitchScreen(uint16_t parameter)
{
	uint16_t cmd,param,crc16;
	tx_num = sizeof(cmd) + sizeof(parameter)+sizeof(crc16);

	cmd  = CharReverse16(0xB100);
	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));

	param = CharReverse16(parameter);
	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter));

	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
	crc16 = CharReverse16(crc16);
	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));

	FRAME_SEND(dacai_head,dacai_tail);
}


void CMD_SetButtonStatus(uint16_t parameter1,uint16_t parameter2,uint8_t parameter3)
{
	uint16_t cmd,param,crc16;
	tx_num = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(parameter3)+sizeof(crc16);

	cmd  = CharReverse16(0xB110);
	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));

	param = CharReverse16(parameter1);
	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter1));

	param = CharReverse16(parameter2);
	memcpy(&tx_buffer[4],(uint8_t*)&param,sizeof(parameter2));

	param = CharReverse16(parameter3);
	memcpy(&tx_buffer[6],(uint8_t*)&parameter3,sizeof(parameter3));

	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));

	FRAME_SEND(dacai_head,dacai_tail);
}



void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2)
{
	uint16_t cmd,param,crc16;
	tx_num = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(crc16);

	cmd  = CharReverse16(0xB111);
	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));

	param = CharReverse16(parameter1);
	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter1));

	param = CharReverse16(parameter2);
	memcpy(&tx_buffer[4],(uint8_t*)&param,sizeof(parameter2));

	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));

	FRAME_SEND(dacai_head,dacai_tail);
}








void PConectRceive(void)
{
	if(HEAD_VERIFICATION(pc_head))
	{
		if(TIAL_VERIFICATION(pc_tail))
		{
			PC_Conect_t* pstruct = (PC_Conect_t*)&rx_buffer[sizeof(pc_head)];
			pc_connect.mode = pstruct->mode;
			pc_connect.data_len = pstruct->data_len;
			pc_connect.addr = pstruct->addr;
			pc_connect.addr_num = pstruct->addr_num;
			pc_connect.crc16 = pstruct->crc16;


		}
	}
}




void PConectSend(void)
{
	uint16_t i,crc16;

	tx_buffer[0] = 0x81;
	tx_buffer[1] = (pc_connect.addr_num + 1+1+2+2);
	tx_buffer[2] = (pc_connect.addr&0x00FF);
	tx_buffer[3] = (pc_connect.addr&0xFF00)>>8;




	for(i = 0;i < pc_connect.addr_num;i++)
	{
		AT24Read(pc_connect.addr+i,&tx_buffer[4],i);
	}

	tx_num = tx_buffer[1];
	crc16 = ModBusCRC16(tx_buffer,tx_num-sizeof(crc16));
	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));

	FRAME_SEND(pc_head,pc_tail);
}








void pc_test(void)
{
	PConectRceive();
	PConectSend();
}

























void instruction_decode(void)
{
	char data_index = sizeof(dacai_head)-1;
	if(rx_buffer[data_index+1] == 0xB1)
	{
		switch (rx_buffer[data_index+2])
		{
			case 0x00:		break;
			case 0x01:		break;
			case 0x02:		break;
			case 0x03:printf("0x03  two cmd\r\n");		break;
			case 0x04:		break;
			default:break;
		}
	}else{
		switch (rx_buffer[data_index+1])
		{
			case 0x01:		break;
			case 0x02:		break;
			case 0x03:		break;
			case 0x04:printf("0x04  one cmd\r\n");		break;
			default:break;
		}
	}
}


