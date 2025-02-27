#ifndef __ProtocolProcessing_H
#define __ProtocolProcessing_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usart.h"

#define BUF_SIZE 128
#define HEAD_SIZE 1
#define TAIL_SIZE 4
#define DATA_BUFF_SIZE 21

extern uint8_t rx_buffer[BUF_SIZE];
extern uint8_t tx_buffer[BUF_SIZE];
extern uint8_t rx_num,tx_num;


extern uint8_t frame_head[HEAD_SIZE];
extern uint8_t frame_tail[TAIL_SIZE];
extern uint8_t frame_data[DATA_BUFF_SIZE];

void frame_send(uint8_t* frame_data);
void rx_resend(void);
void instruction_decode(void);
void instruction_code(void);
void CMD_ReadScreen(void);


#endif

#ifdef __cplusplus
}
#endif
