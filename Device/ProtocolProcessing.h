#ifndef __ProtocolProcessing_H
#define __ProtocolProcessing_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usart.h"
#include "SoftwareCRC.h"

#define BUF_SIZE 128


extern uint8_t rx_buffer[BUF_SIZE];
extern uint8_t tx_buffer[BUF_SIZE];
extern uint8_t rx_num;
extern uint8_t tx_num;
extern uint8_t pc_head[];
extern uint8_t pc_tail[];


void frame_send(uint8_t* head,uint8_t* tail,uint8_t head_size,uint8_t tail_size);
void pc_rx_process(void);
void instruction_decode(void);
void instruction_code(void);
void CMD_ReadScreen(void);
void CMD_SwitchScreen(uint16_t parameter);
void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2);


typedef struct{
	uint8_t mode;
	uint8_t data_len;
	uint16_t addr;
	uint16_t addr_num;
	uint16_t crc16;
}PC_Conect_t;


extern PC_Conect_t pc_connect;

void pc_test(void);


#endif

#ifdef __cplusplus
}
#endif
