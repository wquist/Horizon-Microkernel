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

/*! \file core/multitask/scheduler.h
 *  \date September 2014
 */

#pragma once

#include <multitask/thread.h>
#include <horizon/types.h>

#define SCHEDULER_FREQ 100
#define SCHEDULER_TIMESLICE 5

void scheduler_start();

void scheduler_add(thread_uid_t uid);
void scheduler_remove(thread_uid_t uid);
void scheduler_purge(pid_t pid);

void scheduler_next();
void scheduler_lock();
void scheduler_unlock();

thread_uid_t scheduler_curr();
