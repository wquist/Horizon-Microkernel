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

/*! \file core/memory/virtual.h
 *  \date July 2014
 */

#pragma once

#include <multitask/process.h>
#include <stddef.h>
#include <stdint.h>

//! Generic flags for virtual memory mapping.
/*! Not all flags apply depending on the platform. */
typedef enum virtual_flags VIRTUAL_FLAGS;
enum virtual_flags
{
	VIRTUAL_FLAG_READ  = 0x1,
	VIRTUAL_FLAG_WRITE = 0x2,
	VIRTUAL_FLAG_EXEC  = 0x4
};

void virtual_map(process_t* proc, uintptr_t virt, void* phys, size_t size, VIRTUAL_FLAGS flags);
void virtual_unmap(process_t* proc, uintptr_t virt, size_t size);
int  virtual_is_mapped(process_t* proc, uintptr_t virt, size_t size);

void virtual_alloc(process_t* proc, uintptr_t virt, size_t size, VIRTUAL_FLAGS flags);
void virtual_clone(process_t* dest, process_t* src, uintptr_t virt, size_t size, VIRTUAL_FLAGS flags);
