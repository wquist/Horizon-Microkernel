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

/*! \file multiboot.h
 *  \date June 2014
 *
 *  \note View the official documentation at 
 *  [gnu.org](http://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
 */

#pragma once

#define MULTIBOOT_HEADER

//! Various constants used in the multiboot specification.
typedef enum multiboot_defs MULTIBOOT_DEFS;
enum multiboot_defs
{
    MULTIBOOT_HEADER_MAGIC = 0x1BADB002, //!< The magic defined in the bootstrap.
    MULTIBOOT_LOADER_MAGIC = 0x2BADB002, //!< The magic that can be checked in the kernel main.

    MULTIBOOT_UNSUPPORTED  = 0x0000FFFC,
    MULTIBOOT_MOD_ALIGN    = 0x00001000,
    MULTIBOOT_INFO_ALIGN   = 0x00000004,

    MULTIBOOT_PAGE_ALIGN   = 0x00000001,

    MULTIBOOT_MEMORY_INFO  = 0x00000002,
    MULTIBOOT_VIDEO_MODE   = 0x00000004,
    MULTIBOOT_AOUT_KLUDGE  = 0x00010000
};

//! Various flags from the multiboot spec.
typedef enum multiboot_flags MULTIBOOT_FLAGS;
enum multiboot_flags
{
    MULTIBOOT_INFO_MEMORY           = 0x00000001, //!< Flag defined if the loader properly detected RAM.
    MULTIBOOT_INFO_BOOTDEV          = 0x00000002,
    MULTIBOOT_INFO_CMDLINE          = 0x00000004,
    MULTIBOOT_INFO_MODS             = 0x00000008, //!< Flag defined if modules were loaded with the kernel.

    MULTIBOOT_INFO_AOUT_SYMS        = 0x00000010,
    MULTIBOOT_INFO_ELF_SHDR         = 0x00000020,

    MULTIBOOT_INFO_MEM_MAP          = 0x00000040, //!< Flag defined if the loader created a memory map.
    MULTIBOOT_INFO_DRIVE_INFO       = 0x00000080,
    MULTIBOOT_INFO_CONFIG_TABLE     = 0x00000100,
    MULTIBOOT_INFO_BOOT_LOADER_NAME = 0x00000200,
    MULTIBOOT_INFO_APM_TABLE        = 0x00000400,
    MULTIBOOT_INFO_VIDEO_INFO       = 0x00000800
};

//! Possible types of memory regions.
typedef enum multiboot_memory MULTIBOOT_MEMORY;
enum multiboot_memory
{
    MULTIBOOT_MEMORY_AVAILABLE = 0x1,
    MULTIBOOT_MEMORY_RESERVED  = 0x2
};

typedef unsigned short     multiboot_uint16_t;
typedef unsigned int       multiboot_uint32_t;
typedef unsigned long long multiboot_uint64_t;

typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;
struct multiboot_aout_symbol_table
{
    multiboot_uint32_t tabsize;
    multiboot_uint32_t strsize;
    multiboot_uint32_t addr;
    multiboot_uint32_t reserved;
};

typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;
struct multiboot_elf_section_header_table
{
    multiboot_uint32_t num;
    multiboot_uint32_t size;
    multiboot_uint32_t addr;
    multiboot_uint32_t shndx;
};

//! The main multiboot structure.
/*! \note Before most values can be used, the appropriate
 *  flag must be checked to make sure the values
 *  were properly defined and are available.
 *
 *  This structure is created by the loader and
 *  passed to the kernel. It contains all info
 *  gathered by the loader; most importantly,
 *  memory info and module addresses.
 */
typedef struct multiboot_info multiboot_info_t;
struct multiboot_info
{
    multiboot_uint32_t flags; //!< Values defined in MULTIBOOT_FLAGS.
    
    multiboot_uint32_t mem_lower; //!< Low memory, up to 640 KB.
    multiboot_uint32_t mem_upper; //!< High memory, the maximum available address minus 1 MB.
    
    multiboot_uint32_t boot_device;
    
    multiboot_uint32_t cmdline;
    
    multiboot_uint32_t mods_count;
    multiboot_uint32_t mods_addr;
    
    union
    {
        multiboot_aout_symbol_table_t aout_sym;
        multiboot_elf_section_header_table_t elf_sec;
    };
    
    multiboot_uint32_t mmap_length;
    multiboot_uint32_t mmap_addr;
    
    multiboot_uint32_t drives_length;
    multiboot_uint32_t drives_addr;
    
    multiboot_uint32_t config_table;
    
    multiboot_uint32_t boot_loader_name;
    
    multiboot_uint32_t apm_table;
    
    multiboot_uint32_t vbe_control_info;
    multiboot_uint32_t vbe_mode_info;
    multiboot_uint16_t vbe_mode;
    multiboot_uint16_t vbe_interface_seg;
    multiboot_uint16_t vbe_interface_off;
    multiboot_uint16_t vbe_interface_len;
};

//! A multiboot memory map entry.
/*! \note In memory, this struct is aligned at addr; i.e. size is at 
 *  location -4, addr is at 0, etc.
 */
typedef struct multiboot_mmap_entry multiboot_memory_map_t;
struct __attribute__((packed)) multiboot_mmap_entry
{
    multiboot_uint32_t size;
    multiboot_uint64_t addr;
    multiboot_uint64_t length;
    /*! \brief Memory type (MULTIBOOT_MEMORY_*)
     *
     *  A memory map entry can only be used by the kernel
     *  if its type equals 1. However, other values hint
     *  at the type of memory: 2 is general reserved, 3 and
     *  4 are ACPI memory, and 5 is damaged memory.
     */
    multiboot_uint32_t type;
};

typedef struct multiboot_mod_list multiboot_module_t;
struct multiboot_mod_list
{
    multiboot_uint32_t mod_start;
    multiboot_uint32_t mod_end;
    
    multiboot_uint32_t cmdline;
    
    multiboot_uint32_t pad;
};
