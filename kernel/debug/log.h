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

/*! \file debug/log.h
 *  \date June 2014
 */

#pragma once

#if defined(_DEBUG)
#define dtrace(x, ...) do { debug_trace(__FILE__, x, ##__VA_ARGS__); } while (0)
#else
#define dtrace(x, ...) do {} while (0)
#endif

void debug_trace(const char* file, const char* format, ...);
