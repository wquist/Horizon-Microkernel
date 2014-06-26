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

/*! \file arch/i586/sw/int/isr.h
 *  \date June 2014
 */

#pragma once

#include <stdint.h>

#define ISR_MAX 256

typedef uint8_t isr_t;

//! Each leading hex digit (0x00,0x10,0x20) has a priority level.
/*! The levels are divided into 4 main priorities. */
typedef enum isr_priority ISR_PRIORITY;
enum isr_priority
{
	ISR_PRIORITY_CRITICAL = 0,
	ISR_PRIORITY_HIGH     = 4,
	ISR_PRIORITY_NORMAL   = 8,
	ISR_PRIORITY_LOW      = 12,

	ISR_PRIORITY_LOWEST   = 15,
	ISR_PRIORITY_LEVELS
};

void  isr_reserve(isr_t isr);
isr_t isr_alloc(ISR_PRIORITY pri);
void  isr_free(isr_t isr);
