#ifndef __ProtocolProcessing_H
#define __ProtocolProcessing_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usart.h"

#define BUF_SIZE (128+1)


extern uint8_t rx_buffer[BUF_SIZE];
extern uint8_t tx_buffer[BUF_SIZE];



void frame_send(uint8_t* head,uint8_t* tail,uint8_t head_size,uint8_t tail_size);
void pc_rx_process(void);
void instruction_decode(void);
void instruction_code(void);
void CMD_ReadScreen(void);
void CMD_SwitchScreen(uint16_t parameter);
void CMD_temp(uint16_t parameter1,uint16_t parameter2);


typedef struct{
	uint8_t mode;
	uint8_t data_len;
	uint8_t addr_L;
	uint8_t addr_H;
	uint8_t addr_num_L;
	uint8_t addr_num_H;
	uint32_t CRC_Check;
}PC_Conect_t;





#endif

#ifdef __cplusplus
}
#endif
