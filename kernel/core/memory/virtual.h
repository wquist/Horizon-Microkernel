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

#include <horizon/types.h>
#include <stddef.h>
#include <stdint.h>

/*! All of the virtual memory management functions operate on the passed
 *    process's address space, or the kernel PAS if NULL is passed.
 *    Always pass NULL for kernel mappings.
 *  Passed addresses and sizes are automatically page aligned.
 *  If a page is mapped inside the region being operated on, it is ignored,
 *    i.e., it will not be overwritten/cloned etc.
 */

void virtual_map(pid_t pid, uintptr_t virt, const void* phys, size_t size);
void virtual_unmap(pid_t pid, uintptr_t virt, size_t size);
int  virtual_is_mapped(pid_t pid, uintptr_t virt, size_t size);

void virtual_alloc(pid_t pid, uintptr_t virt, size_t size);
void virtual_share(pid_t dest, pid_t src, uintptr_t to, uintptr_t from, size_t size, uint8_t mode);
