#ifndef __ProtocolProcessing_H
#define __ProtocolProcessing_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usart.h"
#include "SoftwareCRC.h"

#define BUF_SIZE 128
#define CMD_SEND(command, ...) CMD_Send(command,##__VA_ARGS__, 0xFFFF)
#define FRAME_SEND_USART huart1


extern uint8_t rx_buffer[BUF_SIZE];
extern uint8_t tx_buffer[BUF_SIZE];
extern uint8_t rx_num;
extern uint8_t tx_num;


/* Host computer communication protocol format */
typedef struct{
	uint8_t mode;
	uint8_t data_len;
	uint16_t addr;
	uint16_t addr_num;
	uint16_t crc16;
}PC_Conect_t;
extern PC_Conect_t pc_connect;

/* Dacai screen control command */
typedef enum{
	 ReadButtonStatus = 0xB111,
	 SetButtonStatus = 0xB110,
	 SwitchScreen = 0xB100,
}Cmd_t;
extern Cmd_t cmd;








void pc_rx_process(void);
void instruction_decode(void);
void instruction_code(void);

//void CMD_ReadScreen(void);
//void CMD_SwitchScreen(uint16_t parameter);
//void CMD_ReadButtonStatus(uint16_t parameter1,uint16_t parameter2);
//void CMD_ReadButtonStatus_Improve(uint16_t command, ...);

extern void CMD_Send(uint16_t command, ...);


void PConectProcess(void);


#endif

#ifdef __cplusplus
}
#endif
