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

/*! \file arch/i586/arch.h
 *  \date June 2014
 */

#pragma once

#include <boot/meminfo.h>
#include <boot/module.h>

#include <sw/paging.h>
#include <sw/task.h>
#include <sw/int/frame.h>
#include <sw/int/ctl.h>
#include <sw/instr/io.h>
#include <hw/console.h>

#include <spec/multiboot.h>
#include <stdint.h>

//! Define the active achitecture - may be useful elsewhere.
#define ARCH_X86
#define ARCH_ISA_I586

//! Redfine multiboot variables into generic names.
typedef struct multiboot_info bootloader_info_t;
#define BOOTLOADER_MAGIC MULTIBOOT_LOADER_MAGIC

//! Address constant taken as the address of variables in the linker script.
extern const uintptr_t KERNEL_PHYS_ADDR, KERNEL_VIRT_BASE, KERNEL_VIRT_ADDR;
extern const uintptr_t __skernel, __ekernel;

//! The lowest safe address to start allocating memory from.
#define PHYSICAL_USABLE_BASE 0x1000000
//! The architecture's page size.
#define ARCH_PGSIZE PAGING_PAGE_SIZE

//! Redefine the top-level paging structure.
typedef struct paging_dir paging_pas_t;
//! Make the structure alloc/free generic too.
#define paging_pas_create  paging_dir_create
#define paging_pas_destroy paging_dir_destroy
//! As well as the switching.
#define paging_pas_load paging_dir_load

void arch_init();
