/*
 * simple utility to get a hex value from a nibble
 *
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

#include "util.h"

char to_hex(char byte, char high)
{
	char nibble;

	if (high) {
		nibble = (byte & 0xF0) >> 4;
	} else {
		nibble = (byte & 0x0F);
	}

	if (nibble < 10) {
		return '0' + nibble;
	}
	return 'A' + nibble - 10;
}
