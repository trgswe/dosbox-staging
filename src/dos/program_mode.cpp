/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2021-2021  The DOSBox Staging Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "program_mode.h"

#include <string>
#include <map>
#include <functional>

#include "callback.h"
#include "regs.h"
#include "shell.h"
#include "string_utils.h"

#include "../ints/int10.h"

static void set_mode_80x50();

static std::map<std::string, std::function<void()>> modes = {
        {"40x25", []() { INT10_SetVideoMode(0x1); }},
        {"80x25", []() { INT10_SetVideoMode(0x3); }},
        {"80x50", set_mode_80x50},
        {"80x60", [] { INT10_SetVideoMode(0x108); }},
        {"132x25", []() { INT10_SetVideoMode(0x109); }},
        {"132x43", []() { INT10_SetVideoMode(0x10a); }},
};

static bool mode_exists(const std::string key)
{
	return modes.count(key) == 1;
}

static void set_mode_80x50()
{
	reg_ah = 0x12; // EGA/VGA special functions
	reg_al = 0x02; // 400 scan lines (VGA only)
	reg_bl = 0x30; // Set text mode scan lines
	CALLBACK_RunRealInt(0x10);

	reg_ax = 0x0003; // Set 80x25 video mode
	CALLBACK_RunRealInt(0x10);

	reg_ax = 0x1112; // Load and activate ROM 8x8 character set
	CALLBACK_RunRealInt(0x10);

	reg_ah = 0x01; // Set cursor shape and size
	reg_ch = 0x06; // start / upper row of character matrix
	reg_cl = 0x07; // end / lower row of character matrix
	CALLBACK_RunRealInt(0x10);
}

void MODE::Run()
{
	std::string args = "";
	cmd->GetStringRemain(args);

	if (mode_exists(args)) {
		const auto set_mode = modes[args];
		set_mode();
	} else {
		WriteOut("Text mode '%s' is not available.\n"
		         "Supported text modes: ", args.c_str());
		for (const auto &pair : modes) {
			WriteOut("%s ", pair.first.c_str());
		}
		WriteOut("\n");
	}
}

void MODE_ProgramStart(Program **make)
{
	*make = new MODE;
}
