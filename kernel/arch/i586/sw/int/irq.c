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

#include "irq.h"
#include <debug/error.h>

static irq_t irq_map[ISR_MAX] = {-1};
static isr_t isr_map[IRQ_MAX] = {0};

//! Associate the given IRQ with an ISR.
void irq_set_isr(irq_t irq, isr_t isr)
{
	dassert(irq_map[isr] == -1);
	dassert(irq >= 0);

	irq_map[isr] = irq;
	isr_map[irq] = isr;
}

//! Get an IRQ based on its ISR number.
irq_t irq_from_isr(isr_t isr)
{
	return irq_map[isr];
}

//! Get an ISR based on its IRQ number.
isr_t irq_to_isr(irq_t irq)
{
	dassert(irq >= 0);
	return isr_map[irq];
}
