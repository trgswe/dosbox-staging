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

#include "string_utils.h"

#include <gtest/gtest.h>

#include <string>

#include "support.h"

namespace {

TEST(SafeStrcpy, SimpleCopy)
{
	char buffer[10] = "";
	char *ret_value = safe_strcpy(buffer, "abc");
	EXPECT_EQ(ret_value, &buffer[0]);
	EXPECT_STREQ("abc", buffer);
}

TEST(SafeStrcpy, CopyFromNonArray)
{
	char buffer[10] = "";
	std::string str = "abc";
	EXPECT_STREQ("abc", safe_strcpy(buffer, str.c_str()));
}

TEST(SafeStrcpy, EmptyStringOverwrites)
{
	char buffer[4] = "abc";
	EXPECT_STREQ("", safe_strcpy(buffer, ""));
}

TEST(SafeStrcpy, StringLongerThanBuffer)
{
	char buffer[5] = "";
	char long_input[] = "1234567890";
	ASSERT_LT(ARRAY_LEN(buffer), strlen(long_input));
	EXPECT_STREQ("1234", safe_strcpy(buffer, long_input));
}

TEST(SafeStrcpy_DeathTest, PassNull)
{
	char buf[] = "12345678";
	EXPECT_DEBUG_DEATH({ safe_strcpy(buf, nullptr); }, "");
}

TEST(SafeStrcpy_DeathTest, ProtectFromCopyingOverlappingString)
{
	char buf[] = "12345678";
	char *overlapping = &buf[2];
	ASSERT_LE(buf, overlapping);
	ASSERT_LE(overlapping, buf + ARRAY_LEN(buf));
	EXPECT_DEBUG_DEATH({ safe_strcpy(buf, overlapping); }, "");
}

TEST(SafeStrcpy_4, SimpleCopy)
{
	char buffer[10] = "123456789";
	char *ret_value = safe_strcpy(buffer, 10, "abc", strlen("abc"));
	EXPECT_EQ(ret_value, &buffer[0]);
	EXPECT_STREQ("abc", buffer);
}

TEST(SafeStrcpy_4, StringLongerThanBuffer)
{
	char buffer[4] = "";
	char long_input[] = "1234567890";
	const size_t buf_len = ARRAY_LEN(buffer);
	const size_t str_len = strlen(long_input);
	ASSERT_LT(buf_len, str_len);
	EXPECT_STREQ("123", safe_strcpy(buffer, buf_len, long_input, str_len));
}

TEST(SafeStrcpy_4, EmptyStringOverwrites)
{
	char buffer[4] = "abc";
	EXPECT_STREQ("", safe_strcpy(buffer, 4, "", 0));
}

TEST(SafeStrcpy_4, CopyToNonArray)
{
	auto *buffer = new char[4];
	std::string str = "abc";
	ASSERT_EQ(3, str.size());
	EXPECT_STREQ("abc", safe_strcpy(buffer, 4, str.c_str(), str.size()));
	delete [] buffer;
}

TEST(SafeStrcpy_4, CopyPrefixOfLongerString)
{
	char buffer[4] = "";
	char long_input[] = "abc";
	const size_t buf_len = ARRAY_LEN(buffer);
	const size_t to_copy = 2;
	ASSERT_LT(to_copy, strlen(long_input));
	EXPECT_STREQ("ab", safe_strcpy(buffer, buf_len, long_input, to_copy));
}

} // namespace
