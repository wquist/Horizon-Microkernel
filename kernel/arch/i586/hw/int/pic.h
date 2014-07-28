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

/*! \file arch/i586/hw/int/pic.h
 *  \date June 2014
 */

#pragma once

#include <sw/int/isr.h>
#include <sw/int/irq.h>
#include <spec/pic8259.h>
#include <spec/pic8259/x86.h>
#include <stdint.h>
#include <stdbool.h>

#define PIC_REMAP_BASE 32

void pic_init();
void pic_remap(isr_t pic1, isr_t pic2);
