#ifndef __SOFTWARECRC_H
#define __SOFTWARECRC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

uint32_t Soft_CRC_Calculate(uint8_t* data, uint16_t len);


#endif

#ifdef __cplusplus
}
#endif
