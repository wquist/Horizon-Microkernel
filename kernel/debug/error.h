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

/*! \file debug/error.h
 *  \date June 2014
 */

#pragma once

#include <debug/log.h>

#if defined(_DEBUG)

#define dassert(c) \
	do { if (c); else {                                             \
	debug_trace(__FILE__, "Assertion failure in %s, line %u: (%s)", \
	            __PRETTY_FUNCTION__, __LINE__, #c);                 \
	for (;;); } } while (0)

#define dpanic(x) do { debug_trace(__FILE__, "PANIC: %s", x); for (;;); } while (0)

#else

#define dassert(c) do {} while (0)
#define dpanic(x)  do {} while (0)

#endif
