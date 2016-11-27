#ifndef _OPENCM3UTIL_H_
#define _OPENCM3UTIL_H_

#include <stdint.h>
#include <libopencm3/stm32/f4/spi.h>

uint32_t spi_read_mode_fault(uint32_t spi);
void spi_clear_mode_fault(uint32_t spi);

#endif /* _OPENCM3UTIL_H_ */
