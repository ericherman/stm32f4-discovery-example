/* opencm3util.h
   Copyright (C) 2012, 2016, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
#ifndef _OPENCM3UTIL_H_
#define _OPENCM3UTIL_H_

#include <stdint.h>
#include <libopencm3/stm32/f4/spi.h>

uint32_t spi_read_mode_fault(uint32_t spi);
void spi_clear_mode_fault(uint32_t spi);

#endif /* _OPENCM3UTIL_H_ */
