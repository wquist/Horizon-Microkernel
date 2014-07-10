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

typedef enum virtual_clone_mode VIRTUAL_CLONE_MODE;
enum virtual_clone_mode
{
	VIRTUAL_CLONE_READ,
	VIRTUAL_CLONE_WRITE
};

/*! All of the virtual memory management functions operate on the passed
 *    process's address space, or the kernel PAS if NULL is passed.
 *    Always pass NULL for kernel mappings.
 *  Passed addresses and sizes are automatically page aligned.
 *  If a page is mapped inside the region being operated on, it is ignored,
 *    i.e., it will not be overwritten/cloned etc.
 */

void virtual_map(process_t* proc, uintptr_t virt, const void* phys, size_t size);
void virtual_unmap(process_t* proc, uintptr_t virt, size_t size);
int  virtual_is_mapped(const process_t* proc, uintptr_t virt, size_t size);

void virtual_alloc(process_t* proc, uintptr_t virt, size_t size);
void virtual_clone(process_t* dest, const process_t* src, uintptr_t from, uintptr_t to, size_t size, VIRTUAL_CLONE_MODE mode);
