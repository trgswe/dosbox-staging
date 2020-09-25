/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  Copyright (C) 2020-2020  The dosbox-staging team
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

#include "mem.h"

#include <gtest/gtest.h>

#include "control.h"
#include "paging.h"
#include "setup.h"

namespace {

static void DOSBOX_FakeInit(Section *sec)
{}

class MemoryAccessTest : ::testing::Test {
protected:
	MemoryAccessTest()
	{
		Section_prop *secprop;
		secprop = control->AddSection_prop("dosbox", &DOSBOX_FakeInit);
		secprop->AddInitFunction(&PAGING_Init); // done
		secprop->AddInitFunction(&MEM_Init); // done
		Prop_int* pint;
		pint = secprop->Add_int("memsize",
		                        Property::Changeable::WhenIdle, 16);
		pint->SetMinMax(1, 63);
		//PAGING_InitTLB();
		//PAGING_Enable(true);
		//PAGING_ForcePageInit();
		//MEM_Init();
	}

	~MemoryAccessTest() {}
};

TEST(MemoryAccessTest, ReadWriteByte_NominalLow)
{
	const uint16_t seg = 64;
	const uint16_t off = 64;
	const uint8_t val = 16;
	mem_writeb((seg << 4) + off, val);

	const auto original = mem_readb((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writeb(seg, off, val);
	const auto current = real_readb(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

} // namespace
