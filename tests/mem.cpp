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
#include "dos_inc.h"

#include <gtest/gtest.h>

namespace {

TEST(Mem_RealRW_B, NominalLow)
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

TEST(Mem_RealRW_B, NominalMid)
{
	const uint16_t seg = 500;
	const uint16_t off = 500;
	const uint8_t val = 32;
	mem_writeb((seg << 4) + off, val);

	const auto original = mem_readb((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writeb(seg, off, val);
	const auto current = real_readb(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_B, NominalLarge)
{
	const uint16_t seg = 1000;
	const uint16_t off = 1000;
	const uint8_t val = 128;
	mem_writeb((seg << 4) + off, val);

	const auto original = mem_readb((seg<<4)+off);
	EXPECT_EQ(original, val);

	real_writeb(seg, off, val);
	const auto current = real_readb(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_B, IllegalWrap)
{
	const uint16_t seg = 2000;
	const uint16_t off = 2000;
	EXPECT_DEBUG_DEATH({ real_readb(seg, off); }, "");
}

TEST(Mem_RealRW_W, NominalLow)
{
	const uint16_t seg = 64;
	const uint16_t off = 64;
	const uint16_t val = 16;
	mem_writew((seg << 4) + off, val);

	const auto original = mem_readw((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writew(seg, off, val);
	const auto current = real_readw(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_W, NominalMid)
{
	const uint16_t seg = 500;
	const uint16_t off = 500;
	const uint16_t val = 500;

	mem_writew((seg << 4) + off, val);
	const auto original = mem_readw((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writew(seg, off, val);
	const auto current = real_readw(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_W, NominalLarge)
{
	const uint16_t seg = 1000;
	const uint16_t off = 1000;
	const uint16_t val = 16000;
	mem_writew((seg << 4) + off, val);

	const auto original = mem_readw((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writew(seg, off, val);
	const auto current = real_readw(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_W, IllegalWrap)
{
	const uint16_t seg = 2000;
	const uint16_t off = 2000;
	EXPECT_DEBUG_DEATH({ real_readw(seg, off); }, "");
}

TEST(Mem_RealRW_D, NominalLow)
{
	const uint16_t seg = 64;
	const uint16_t off = 64;
	const uint32_t val = 16;
	mem_writed((seg << 4) + off, val);

	const auto original = mem_readd((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writed(seg, off, val);
	const auto current = real_readd(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_D, NominalMid)
{
	const uint16_t seg = 500;
	const uint16_t off = 500;
	const uint32_t val = 300000;
	mem_writed((seg << 4) + off, val);

	const auto original = mem_readd((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writed(seg, off, val);
	const auto current = real_readd(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_D, NominalLarge)
{
	const uint16_t seg = 1000;
	const uint16_t off = 1000;
	const uint32_t val = 5000000;
	mem_writed((seg << 4) + off, val);

	const auto original = mem_readd((seg << 4) + off);
	EXPECT_EQ(original, val);

	real_writed(seg, off, val);
	const auto current = real_readd(seg, off);
	EXPECT_EQ(current, val);

	EXPECT_EQ(original, current);
}

TEST(Mem_RealRW_D, IllegalWrap)
{
	const uint16_t seg = 2000;
	const uint16_t off = 2000;
	EXPECT_DEBUG_DEATH({ real_readb(seg, off); }, "");
}

} // namespace
