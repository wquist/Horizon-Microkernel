/* Horizon Microkernel - Hobby Operating System
 * Copyright (C) 2014 Wyatt Lindquist
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

//! Print a kernel message on a new line.
/*! Print a formatted message, prepended with the calling filename. */
void debug_trace(const char* file, const char* format, ...)
{
	// Extract the filename from the path
	char* s = strrchr(file, '/');
	if (s) ++s;

	// Extract the filename from the extension
	char* e = strchr(file, '.');
	size_t slen = e - s;

	printf("[");
	while (slen--) printf("%c", *s++);
	printf("]");

	va_list arg;
	va_start(arg, format);
	vprintf(format, &arg);
	va_end(arg);

	printf("\n");
}
