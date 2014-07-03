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

/*! \file core/multitask/process.h
 *  \date July 2014
 */

#pragma once

#include <arch.h>
#include <util/bitmap.h>
#include <stddef.h>
#include <stdint.h>

#define PROCESS_MAX (2<<15)
#define PROCESS_BLOCK_SIZE 4096
#define PROCESS_THREAD_MAX 1024

typedef struct process process_t;
struct process
{
	uint16_t pid, parent;
	size_t priv;

	uintptr_t entry;
	paging_pas_t* addr_space;

	struct
	{
		size_t count;
		uint16_t slots[PROCESS_THREAD_MAX];
		bitmap_t bitmap[BITMAP_LENGTH(PROCESS_THREAD_MAX)];
	} threads;
};

//
