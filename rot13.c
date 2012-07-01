/*
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

#include "rot13.h"

void rotate_letters(char *buf, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; ++i) {
		if (buf[i] >= 'a' && buf[i] <= 'm') {
			buf[i] = buf[i] += 13;
		} else if (buf[i] >= 'n' && buf[i] <= 'z') {
			buf[i] = buf[i] -= 13;
		} else if (buf[i] >= 'A' && buf[i] <= 'M') {
			buf[i] = buf[i] += 13;
		} else if (buf[i] >= 'N' && buf[i] <= 'Z') {
			buf[i] = buf[i] -= 13;
		}
	}
}
