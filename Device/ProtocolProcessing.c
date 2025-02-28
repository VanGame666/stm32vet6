#include "Protocolprocessing.h"

uint8_t rx_buffer[BUF_SIZE]={0};
uint8_t tx_buffer[BUF_SIZE]={0};

uint8_t dacai_head[] = {0xEE};
uint8_t dacai_tail[] = {0xFF,0xFC,0xFF,0xFF};

uint8_t pc_head[] = {0x5A,0xA5};
uint8_t pc_tail[] = {0xA5,0x5A};

PC_Conect_t pc_connect;


uint16_t CharReverse16 (uint16_t data)
{
	return (data & 0x00ff) << 8 | (data & 0xff00) >> 8;
}


uint16_t CharReverse32 (uint16_t data)
{
	return (data & 0x000000ff) << 24 | (data & 0x0000ff00) << 8 | (data & 0x00ff0000) >> 8 | (data & 0xff000000 >> 24);
}


#define FRAME_SEND(head,tail) frame_send(head,tail,sizeof(head),sizeof(tail))
void frame_send(uint8_t* head,uint8_t* tail,uint8_t head_size,uint8_t tail_size)
{
	uint8_t frame_size = head_size + tail_size + tx_buffer[0];
	uint8_t frame[frame_size];
	uint8_t i,j = 0;
	for(i = 0;i < head_size;i++,j++)
	{
		frame[j] = head[i];
	}
	for(i = 0;i < tx_buffer[0];i++,j++)
	{
		frame[j] = tx_buffer[1+i];
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
		if(rx_buffer[1+i] != head[i]){return 0;}
	}
	return 1;
}



# define TIAL_VERIFICATION(tail) tail_verification(tail,sizeof(tail))
int tail_verification(uint8_t* tail,uint8_t tail_size)
{
	for(int i = 0;i < tail_size;i++)
	{
		if(rx_buffer[rx_buffer[0]-3+i] != tail[i]){return 0;}
	}
	return 1;
}



void CMD_ReadScreen(void)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB101);
	tx_buffer[0] = sizeof(cmd);
	memcpy(&tx_buffer[1],(uint8_t*)&cmd,sizeof(cmd));
	FRAME_SEND(dacai_head,dacai_tail);
}



void CMD_SwitchScreen(uint16_t parameter)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB100);
	param = CharReverse16(parameter);
	memcpy(&tx_buffer[1],(uint8_t*)&cmd,sizeof(cmd));
	memcpy(&tx_buffer[1+2],(uint8_t*)&param,sizeof(parameter));
	tx_buffer[0] = sizeof(cmd) + sizeof(parameter);
	FRAME_SEND(dacai_head,dacai_tail);
}


void CMD_SetButtonStatus(uint16_t parameter1,uint16_t parameter2,uint8_t parameter3)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB110);
	memcpy(&tx_buffer[1],(uint8_t*)&cmd,sizeof(cmd));
	param = CharReverse16(parameter1);
	memcpy(&tx_buffer[1+2],(uint8_t*)&param,sizeof(parameter1));
	param = CharReverse16(parameter2);
	memcpy(&tx_buffer[1+2+2],(uint8_t*)&param,sizeof(parameter2));
	memcpy(&tx_buffer[1+2+2+2],(uint8_t*)&parameter3,sizeof(parameter3));

	tx_buffer[0] = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(parameter3);
	FRAME_SEND(dacai_head,dacai_tail);
}



void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB111);
	memcpy(&tx_buffer[1],(uint8_t*)&cmd,sizeof(cmd));
	param = CharReverse16(parameter1);
	memcpy(&tx_buffer[1+2],(uint8_t*)&param,sizeof(parameter1));
	param = CharReverse16(parameter2);
	memcpy(&tx_buffer[1+2+2],(uint8_t*)&param,sizeof(parameter2));

	tx_buffer[0] = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2);
	FRAME_SEND(dacai_head,dacai_tail);
}


void PConectFrameSend(void)
{
	memcpy(&tx_buffer[1],(uint8_t*)&pc_connect,sizeof(PC_Conect_t));
	tx_buffer[0] = sizeof(PC_Conect_t);
	FRAME_SEND(pc_head,pc_tail);
}



void PConectFrameRceive(void)
{
	if(HEAD_VERIFICATION(dacai_head))
	{
		if(TIAL_VERIFICATION(dacai_tail))
		{
			PC_Conect_t* pstruct = (PC_Conect_t*)&rx_buffer[sizeof(dacai_head)+1];
			pc_connect.mode = pstruct->mode;
			pc_connect.data_len = pstruct->data_len;
			pc_connect.addr_L = pstruct->addr_L;
			pc_connect.addr_H = pstruct->addr_H;
			pc_connect.addr_num_L = pstruct->addr_num_L;
			pc_connect.addr_num_H = pstruct->addr_num_H;
			pc_connect.CRC_Check = pstruct->CRC_Check;

			printf("rx_data_num= %d \r\n",rx_buffer[0]-sizeof(dacai_head)-sizeof(dacai_tail));
			for(int i = 0;i < rx_buffer[0];i++){printf("0x%2X ",rx_buffer[i+1]);}
			printf("\r\n");
		}
	}
}











void CMD_temp(uint16_t parameter1,uint16_t parameter2)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB100);
	memcpy(&tx_buffer[1],(uint8_t*)&cmd,sizeof(cmd));
	param = CharReverse16(parameter1);
	memcpy(&tx_buffer[1+2],(uint8_t*)&param,sizeof(parameter1));
	param = CharReverse16(parameter2);
	memcpy(&tx_buffer[1+2+2],(uint8_t*)&param,sizeof(parameter2));

	tx_buffer[0] = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2);
	FRAME_SEND(dacai_head,dacai_tail);
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


