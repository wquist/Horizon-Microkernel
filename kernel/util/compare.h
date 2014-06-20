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

/*! \file util/compare.h
 *  \date June 2014
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Replace the size types with intmax types?
static inline size_t min(size_t a, size_t b) { if (a < b) return a; return b; }
static inline size_t max(size_t a, size_t b) { if (a > b) return a; return b; }

static inline ssize_t smin(ssize_t a, ssize_t b) { if (a < b) return a; return b; }
static inline ssize_t smax(ssize_t a, ssize_t b) { if (a > b) return a; return b; }
