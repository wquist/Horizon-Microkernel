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

#pragma once

#include <stdint.h>

enum
{
	PS2CTL_PORT_CMD = 0x64,
	PS2CTL_PORT_DAT = 0x60
};

enum
{
	PS2CTL_CMD_CFGRD    = 0x20,
	PS2CTL_CMD_CFGWR    = 0x60,
	PS2CTL_CMD_DISABLE2 = 0xA7,
	PS2CTL_CMD_ENABLE2  = 0xA8,
	PS2CTL_CMD_TEST2    = 0xA9,
	PS2CTL_CMD_TESTC    = 0xAA,
	PS2CTL_CMD_TEST1    = 0xAB,
	PS2CTL_CMD_DISABLE1 = 0xAD,
	PS2CTL_CMD_ENABLE1  = 0xAE
};

enum
{
	PS2CTL_RES_TESTC_PASS = 0x55,
	PS2CTL_RES_TESTC_FAIL = 0xFC,
	PS2CTL_RES_TESTX_PASS = 0x00,
	PS2CTL_RES_TESTX_CLO  = 0x01,
	PS2CTL_RES_TESTX_CHI  = 0x02,
	PS2CTL_RES_TESTX_DLO  = 0x03,
	PS2CTL_RES_TESTX_DHI  = 0x04
};

union ps2ctl_status
{
	struct __attribute__ ((packed))
	{
		uint8_t out_full : 1;
		uint8_t in_full  : 1;
		uint8_t sysflag  : 1;
		uint8_t ctl_dat  : 1;
		uint8_t          : 2;
		uint8_t time_err : 1;
		uint8_t par_err  : 1;
	};
	uint8_t raw;
};

union ps2ctl_config
{
	struct __attribute__ ((packed))
	{
		uint8_t p1_int    : 1;
		uint8_t p2_int    : 1;
		uint8_t sysflag   : 1;
		uint8_t           : 1;
		uint8_t p1_clock  : 1;
		uint8_t p2_clock  : 1;
		uint8_t translate : 1;
		uint8_t           : 1;
	};
	uint8_t raw;
};

int ps2ctl_init();

int ps2ctl_has_data();
