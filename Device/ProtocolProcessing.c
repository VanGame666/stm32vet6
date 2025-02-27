#include "Protocolprocessing.h"

uint8_t frame_head[HEAD_SIZE] = {0xEE};
uint8_t frame_tail[TAIL_SIZE] = {0xFF,0xFC,0xFF,0xFF};
uint8_t frame_data[DATA_BUFF_SIZE] = {0};//frame_data[0]为发送数据大小

uint8_t rx_buffer[BUF_SIZE]={0};
uint8_t tx_buffer[BUF_SIZE]={0};
uint8_t rx_num = 0,tx_num = 0;

uint16_t CharReverse16 (uint16_t data)
{
	return (data & 0x00ff) << 8 | (data & 0xff00) >> 8;
}


uint16_t CharReverse32 (uint16_t data)
{
	return (data & 0x000000ff) << 24 | (data & 0x0000ff00) << 8 | (data & 0x00ff0000) >> 8 | (data & 0xff000000 >> 24);
}



void frame_send(uint8_t* frame_data)
{
	uint8_t frame_size = HEAD_SIZE+TAIL_SIZE+frame_data[0];
	uint8_t frame[frame_size];
	uint8_t temp_index,frame_index = 0;
	for(temp_index = 0;temp_index < HEAD_SIZE;temp_index++,frame_index++)
	{
		frame[frame_index] = frame_head[temp_index];
	}
	for(temp_index = 0;temp_index < frame_data[0];temp_index++,frame_index++)
	{
		frame[frame_index] = frame_data[1+temp_index];
	}
	for(temp_index = 0;temp_index < TAIL_SIZE;temp_index++,frame_index++)
	{
		frame[frame_index] = frame_tail[temp_index];
	}
	HAL_UART_Transmit(&huart1,&frame,frame_size, 0xFFFF);
}



void CMD_ReadScreen(void)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB101);
	frame_data[0] = sizeof(cmd);
	memcpy(&frame_data[1],(uint8_t*)&cmd,sizeof(cmd));
	frame_send(frame_data);
}

void CMD_SwitchScreen(uint16_t parameter)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB100);
	param = CharReverse16(parameter);
	memcpy(&frame_data[1],(uint8_t*)&cmd,sizeof(cmd));
	memcpy(&frame_data[1+2],(uint8_t*)&param,sizeof(parameter));
	frame_data[0] = sizeof(cmd) + sizeof(parameter);
	frame_send(frame_data);
}


void CMD_SetButtonStatus(uint16_t parameter1,uint16_t parameter2,uint8_t parameter3)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB110);
	memcpy(&frame_data[1],(uint8_t*)&cmd,sizeof(cmd));
	param = CharReverse16(parameter1);
	memcpy(&frame_data[1+2],(uint8_t*)&param,sizeof(parameter1));
	param = CharReverse16(parameter2);
	memcpy(&frame_data[1+2+2],(uint8_t*)&param,sizeof(parameter2));
	memcpy(&frame_data[1+2+2+2],(uint8_t*)&parameter3,sizeof(parameter3));

	frame_data[0] = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(parameter3);
	frame_send(frame_data);
}


void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2)
{
	uint16_t cmd,param;
	cmd  = CharReverse16(0xB111);
	memcpy(&frame_data[1],(uint8_t*)&cmd,sizeof(cmd));
	param = CharReverse16(parameter1);
	memcpy(&frame_data[1+2],(uint8_t*)&param,sizeof(parameter1));
	param = CharReverse16(parameter2);
	memcpy(&frame_data[1+2+2],(uint8_t*)&param,sizeof(parameter2));

	frame_data[0] = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2);
	frame_send(frame_data);
}




int head_verification()
{
	for(int i = 0;i < sizeof(frame_head);i++)
	{
		if(rx_buffer[i] != frame_head[i]){return 0;}
	}
	return 1;
}

int tail_verification()
{
	for(int i = 0;i < sizeof(frame_tail);i++)
	{
		if(rx_buffer[rx_num-4+i] != frame_tail[i]){return 0;}
	}
	return 1;
}


void rx_resend(void)
{
	if(1)
	{
		if(1)
		{
			printf("rx_data_num= %d \r\n",rx_num-HEAD_SIZE-TAIL_SIZE);
			for(int i = 1;i < rx_num-4;i++)
			{
				printf("0x%2X ",rx_buffer[i]);
			}
			printf("\r\n");
		}
	}
}








































void instruction_decode(void)
{
	char data_index = sizeof(frame_head)-1;
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







void instruction_code(void)
{
	char data_index = sizeof(frame_head)-1;
	if(rx_buffer[data_index+1] == 0xB1)
	{
		switch (rx_buffer[data_index+2])
		{
			case 0x00:
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

