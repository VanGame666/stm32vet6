
#ifndef __AT24C02_H
#define __AT24C02_H

#ifdef __cplusplus
 extern "C" {
#endif

/*! -------------------------------------------------------------------------- */
/*! Include headers */
#include "i2c.h"
#include "crc.h"
#include "SoftwareCRC.h"

/*! -------------------------------------------------------------------------- */
/*! Public test functions prototype */
#define AT24CXX_BUFF_SIZE  128

#define AT24CXX_Write_ADDR 0xA0
#define AT24CXX_Read_ADDR  0xA1

#define AT24CXX_MAX_SIZE   256
#define AT24CXX_PAGE_SIZE  8
#define AT24CXX_PAGE_TOTAL (AT24CXX_MAX_SIZE/AT24CXX_PAGE_SIZE)

#define FaultHeader_START 0
#define FaultRecord_START 16
#define FaultRecord_END 128


 extern uint8_t AT24CXX_ReadBuff[];
 extern uint8_t AT24CXX_WriteBuff[];



typedef struct{		//对其16bytes
	uint16_t err_time;
	uint16_t err_data;
	uint16_t fault_count;
	uint16_t read_addr;
	uint16_t write_addr;
	uint32_t CRCheck;
}FaultHeader_t;

typedef struct{  //32bytes
	uint16_t freq;
	uint16_t adc0_val;
	uint16_t adc1_val;
	uint16_t adc2_val;
	uint16_t adc3_val;
	uint16_t adc4_val;
	uint16_t adc5_val;
	uint16_t adc0_threshold_val;
	uint16_t adc1_threshold_val;
	uint16_t adc2_threshold_val;
	uint16_t adc3_threshold_val;
	uint16_t adc4_threshold_val;
	uint16_t adc5_threshold_val;
	uint8_t adc0_err_code;
	uint8_t adc1_err_code;
	uint8_t adc2_err_code;
	uint8_t adc3_err_code;
	uint8_t adc4_err_code;
	uint8_t adc5_err_code;
}FaultRecord_t;

#endif

#ifdef __cplusplus
}
#endif

