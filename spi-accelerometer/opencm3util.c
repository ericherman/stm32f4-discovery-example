/*
 * This extends the libopencm3 library functions.
 *
 * Copyright (C) 2012 Kendrick Shaw <kms15@case.edu>
 * Copyright (C) 2012 Eric Herman <eric@freesa.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/common.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/f4/memorymap.h>
#include "opencm3util.h"

uint32_t spi_read_mode_fault(uint32_t spi)
{
	return SPI_SR(spi) & SPI_SR_MODF;
}

void spi_clear_mode_fault(uint32_t spi)
{
	if (spi_read_mode_fault(spi)) {
		SPI_CR1(spi) = SPI_CR1(spi);
	}
}
