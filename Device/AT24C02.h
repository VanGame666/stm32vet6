
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
#define AT24CXX_BUFF_SIZE  256

#define AT24CXX_Write_ADDR 0xA0
#define AT24CXX_Read_ADDR  0xA1

#define AT24CXX_MAX_SIZE   256
#define AT24CXX_PAGE_SIZE  8
#define AT24CXX_PAGE_TOTAL (AT24CXX_MAX_SIZE/AT24CXX_PAGE_SIZE)
#define FaultHeader_ADD 0
#define FaultRecord_ADD 16

struct StateInfo{
	 uint16_t time;
	 uint16_t data;
	 uint16_t frequency;
	 uint16_t voltage;
	 uint16_t current;
	 uint32_t crcheck;
};

typedef struct{		//对其16bytes
	uint16_t fault_count;
	uint16_t err_data;
	uint16_t err_times;
	uint16_t start_addr;
	uint16_t end_addr;
	uint32_t head_CRCheck;
}FaultHeader;

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
}FaultRecord;

#endif

#ifdef __cplusplus
}
#endif

