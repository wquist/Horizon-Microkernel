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

/*! \file spec/elf.h
 *  \date July 2014
 */

#pragma once

#include <arch.h>
#include <stddef.h>
#include <stdint.h>

//! The load function will only load this many sections.
#define ELF_SECTION_MAX 8

//! Possible values for the 'fid' field in a file header.
typedef enum elf_fid ELF_FID;
enum elf_fid
{
	ELF_FID_MAG = 0, //< The ELF magic numbers.
	ELF_FID_MAG_LENGTH = 4,

	ELF_FID_CLASS = 4,
	ELF_FID_DATA,
	ELF_FID_VERSION,
	ELF_FID_PAD,

	ELF_FID_LENGTH = 16
};

//! Possible values for the 'class' byte of the FID.
typedef enum elf_class ELF_CLASS;
enum elf_class
{
	ELF_CLASS_32 = 1, //!< 32-bit executable.
	ELF_CLASS_64      //!< 64-bit.
};

//! Values for the 'data' byte of the FID.
typedef enum elf_data ELF_DATA;
enum elf_data
{
	ELF_DATA_2LSB = 1, //!< Two's complement LSB.
	ELF_DATA_2MSB,     //!< 2sC MSB.
};
//! Values for the 'type' field in a file header.
typedef enum elf_ftype ELF_FTYPE;
enum elf_ftype
{
	ELF_FTYPE_REL = 1, //!< Relocatable.
	ELF_FTYPE_EXEC,    //!< Executable.
	ELF_FTYPE_DYN,     //!< Shared object.
	ELF_FTYPE_CORE,    //!< 'Core' file.

	ELF_FTYPE_LOPROC = 0xFF00, //!< The start of processor specifics.
	ELF_FTYPE_HIPROC = 0xFFFF  //!< The end of specifics.
};

//! Valid architectures for the 'arch' field in a file header.
typedef enum elf_arch ELF_ARCH;
enum elf_arch
{
	ELF_ARCH_M32 = 1, //!< AT&T WE 32100.
	ELF_ARCH_SPARC,   //!< SPARC.
	ELF_ARCH_386,     //!< Intel 80386.
	ELF_ARCH_68K,     //!< Motorola 68000.
	ELF_ARCH_88K,     //!< Motorola 88000.
	ELF_ARCH_860,     //!< Intel 80860.
	ELF_ARCH_MIPS     //!< MIPS RS3000.
};

//! Program types for the 'type' field in a program header.
typedef enum elf_ptype ELF_PTYPE;
enum elf_ptype
{
	ELF_PTYPE_LOAD = 1, //!< A loadable segment.
	ELF_PTYPE_DYNAMIC,  //!< Dynamic linking info.
	ELF_PTYPE_INTERP,   //!< Program interpreter addr.
	ELF_PTYPE_NOTE,     //!< Extra information segment.
	ELF_PTYPE_SHLIB,    //!< Reserved, but unspecified.
	ELF_PTYPE_PHDR,     //!< Program header table.

	ELF_PTYPE_LOPROC = 0x70000000, //!< Start of proc-reserved values.
	ELF_PTYPE_HIPROC = 0x7FFFFFFF  //!< End of reserved values.
};

//! The format of an ELF file header.
typedef struct elf_file_header elf_file_header_t;
struct __packed elf_file_header
{
	uint8_t fid[ELF_FID_LENGTH];

	uint16_t type;
	uint16_t arch;
	uint32_t version;

	uint32_t entry;
	uint32_t pheader_offset;
	uint32_t sheader_offset;
	uint32_t flags;

	uint16_t size;
	uint16_t pheader_size;
	uint16_t pheader_count;
	uint16_t sheader_size;
	uint16_t sheader_count;
	uint16_t sheader_strtbl;
};

//! The format of an ELF program header.
typedef struct elf_program_header elf_program_header_t;
struct __packed elf_program_header
{
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr, paddr;

	uint32_t file_size;
	uint32_t mem_size;

	uint32_t flags;
	uint32_t align;
};
