/************************************************** START **************************************************/

#include "Protocolprocessing.h"

/* Define the transmit-receive buffer */
uint8_t rx_buffer[BUF_SIZE]={0};
uint8_t tx_buffer[BUF_SIZE]={0};

/* Number of sent and received */
uint8_t rx_num=0;
uint8_t tx_num=0;

/* Communication protocol head and tail frames */
uint8_t dacai_head[] = {0xEE};
uint8_t dacai_tail[] = {0xFF,0xFC,0xFF,0xFF};

uint8_t pc_head[] = {0x5A,0xA5};
uint8_t pc_tail[] = {0xA5,0x5A};

/* Definition of temporary transit of computer communication data */
PC_Conect_t pc_connect;
Cmd_t cmd;






/************************************************** General communication protocol **************************************************/

/**
  * @brief	Data protocol header and tail encapsulation sends
  * @param	Frame head and end of frame
  * @retval	Null
  */
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
	HAL_UART_Transmit(&FRAME_SEND_USART,&frame,frame_size, 0xFFFF);
}



/**
  * @brief	Header verification
  * @param	Frame header
  * @retval Verify that it was successful
  */
# define HEAD_VERIFICATION(head) head_verification(head,sizeof(head))
int head_verification(uint8_t* head,uint8_t head_size)
{
	for(int i = 0;i < head_size;i++)
	{
		if(rx_buffer[i] != head[i]){return 0;}
	}
	return 1;
}


/**
  * @brief	Tail verification
  * @param	Frame tail
  * @retval	Verify that it was successful
  */
# define TIAL_VERIFICATION(tail) tail_verification(tail,sizeof(tail))
int tail_verification(uint8_t* tail,uint8_t tail_size)
{
	for(int i = 0;i < tail_size;i++)
	{
		if(rx_buffer[rx_num-tail_size+i] != tail[i]){return 0;}
	}
	return 1;
}


/************************************************** Dacai Screen Protocol Processing **************************************************/

/**
  * @brief	Single-byte instruction or double-byte instruction is sent,
  * 		and variable number of double-byte parameters are sent at the same time
  * @param	Instructions and parameters must have one instruction, and parameters can be omitted.
  * @retval	Null
  */
void CMD_Send(Cmd_t command, ...)
{
    uint16_t cmd, param, crc16;
    uint16_t count = 0,param_start = 0;
    va_list args;

    /* Determine whether it is a double-byte instruction or a single-byte instruction */
    if((command & 0xFF00) == 0xB100)
    {
    	cmd = CharReverse16((uint16_t)command);
    	memcpy(&tx_buffer[0], (uint8_t*)&cmd, 2);		//It is recommended to determine that the instruction length is incomplete.
        tx_num = sizeof(crc16) + 2;
    	param_start = 2;
    }else{
    	cmd = CharReverse16((uint16_t)command);
    	memcpy(&tx_buffer[0], (uint8_t*)&cmd, 1);
        tx_num = sizeof(crc16) + 1;
    	param_start = 1;
    }

    /* Read variable parameters */
    va_start(args, command);
    while (1) {
        param = (uint16_t)va_arg(args, int);
        if (param == 0xFFFF) break;

        param = CharReverse16((uint16_t)param);
        memcpy(&tx_buffer[param_start + count * sizeof(param)], (uint8_t*)&param, sizeof(param));

        tx_num += sizeof(param);
        count++;
    }
    va_end(args);

    /* Add CRC verification code */
    crc16 = ModBusCRC16(tx_buffer, tx_num - sizeof(crc16));
   	crc16 = CharReverse16(crc16);//The Dacai screen transmission is a big-end transmission, so it should be converted to a big-end mode
    memcpy(&tx_buffer[tx_num - sizeof(crc16)], (uint8_t*)&crc16, sizeof(crc16));

    /* Frame sending */
    FRAME_SEND(dacai_head, dacai_tail);
}


/**
  * @brief
  * @param
  * @retval
  */
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

/************************************************** Upper computer communication **************************************************/

/**
  * @brief	Temporary transfer storage of computer communication data
  * @param	Null
  * @retval	Null
  */
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

			if(pstruct->addr < 12){pc_connect.addr = 0;pc_connect.addr_num = 12;}

		}
	}
}



/**
  * @brief	Send the required data according to the received command of the host computer
  * @param	Null
  * @retval	Null
  */
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


/**
  * @brief	Test
  * @param	Null
  * @retval	Null
  */
void PConectProcess(void)
{
	if(rx_num == 0)return;
	PConectRceive();
	PConectSend();
}



/**
  * @brief	Null
  * @param	Null
  * @retval	Null
  */


/************************************************** Original deprecation scheme **************************************************/

///**
//  * @brief	Serial screen command send
//  * @param	Null
//  * @retval	Null
//  */
//void CMD_ReadScreen(void)
//{
//	uint16_t cmd,param,crc16;
//	tx_num = sizeof(cmd)+sizeof(crc16);
//
//	cmd  = CharReverse16(0xB101);
//	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));
//
//	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
//	crc16 = CharReverse16(crc16);
//	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));
//
//	FRAME_SEND(dacai_head,dacai_tail);
//}
//
//
//void CMD_SwitchScreen(uint16_t parameter)
//{
//	uint16_t cmd,param,crc16;
//	tx_num = sizeof(cmd) + sizeof(parameter)+sizeof(crc16);
//
//	cmd  = CharReverse16(0xB100);
//	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));
//
//	param = CharReverse16(parameter);
//	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter));
//
//	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
//	crc16 = CharReverse16(crc16);
//	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));
//
//	FRAME_SEND(dacai_head,dacai_tail);
//}
//
//
//void CMD_SetButtonStatus(uint16_t parameter1,uint16_t parameter2,uint8_t parameter3)
//{
//	uint16_t cmd,param,crc16;
//	tx_num = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(parameter3)+sizeof(crc16);
//
//	cmd  = CharReverse16(0xB110);
//	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));
//
//	param = CharReverse16(parameter1);
//	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter1));
//
//	param = CharReverse16(parameter2);
//	memcpy(&tx_buffer[4],(uint8_t*)&param,sizeof(parameter2));
//
//	param = CharReverse16(parameter3);
//	memcpy(&tx_buffer[6],(uint8_t*)&parameter3,sizeof(parameter3));
//
//	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
//	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));
//
//	FRAME_SEND(dacai_head,dacai_tail);
//}
//
//
//
//void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2)
//{
//	uint16_t cmd,param,crc16;
//	tx_num = sizeof(cmd) + sizeof(parameter1)+sizeof(parameter2)+sizeof(crc16);
//
//	cmd  = CharReverse16(0xB111);
//	memcpy(&tx_buffer[0],(uint8_t*)&cmd,sizeof(cmd));
//
//	param = CharReverse16(parameter1);
//	memcpy(&tx_buffer[2],(uint8_t*)&param,sizeof(parameter1));
//
//	param = CharReverse16(parameter2);
//	memcpy(&tx_buffer[4],(uint8_t*)&param,sizeof(parameter2));
//
//	crc16 = ModBusCRC16(tx_buffer,tx_num-2);
//	memcpy(&tx_buffer[tx_num-sizeof(crc16)],(uint8_t*)&crc16,sizeof(crc16));
//
//	FRAME_SEND(dacai_head,dacai_tail);
//}


/************************************************** END **************************************************/
