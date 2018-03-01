﻿/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "core/UTF8String.h"
#include "memory.h"

#include <cstdint>
#include <gtest/gtest.h>

using namespace core;

class UTF8StringTest : public testing::Test
{
public:
	void SetUp()
	{

	}

	void TearDown()
	{

	}
};

TEST_F(UTF8StringTest, AStringCanBeInitializedWithAnASCIIString)
{
	UTF8String s("test123");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 't');
	EXPECT_EQ(stringData[1], 'e');
	EXPECT_EQ(stringData[2], 's');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], '1');
	EXPECT_EQ(stringData[5], '2');
	EXPECT_EQ(stringData[6], '3');

	EXPECT_EQ(s.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 7);
}


TEST_F(UTF8StringTest, AStringCanBeInitializedWithFour2ByteCharacters)
{
	UTF8String s("\xD7\xAA\xD7\x95\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], '\xD7'); // 1/2
	EXPECT_EQ(stringData[1], '\xAA'); // 2/2
	EXPECT_EQ(stringData[2], '\xD7'); // 1/2
	EXPECT_EQ(stringData[3], '\x95'); // 2/2
	EXPECT_EQ(stringData[4], '\xD7'); // 1/2
	EXPECT_EQ(stringData[5], '\x93'); // 2/2
	EXPECT_EQ(stringData[6], '\xD7'); // 1/2
	EXPECT_EQ(stringData[7], '\x94'); // 2/2

	EXPECT_EQ(s.getStringLength(), 4);
	EXPECT_EQ(stringData.size(), 8);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedByReplacingInvalidUTF8FirstByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAAy\x95\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();
	EXPECT_EQ(stringData[0], '\xD7');
	EXPECT_EQ(stringData[1], '\xAA');
	EXPECT_EQ(stringData[2], 'y');
	EXPECT_EQ(stringData[3], '\xEF');
	EXPECT_EQ(stringData[4], '\xBF');
	EXPECT_EQ(stringData[5], '\xBD');
	EXPECT_EQ(stringData[6], '\xD7');
	EXPECT_EQ(stringData[7], '\x93');
	EXPECT_EQ(stringData[8], '\xD7');
	EXPECT_EQ(stringData[9], '\x94');

	EXPECT_EQ(s.getStringLength(), 5);
	EXPECT_EQ(stringData.size(), 10);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedByReplacingInvalidUTF8SecondByte)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAA\x95r\xD7\x93\xD7\x94");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], '\xD7');
	EXPECT_EQ(stringData[1], '\xAA');
	EXPECT_EQ(stringData[2], '\xEF');
	EXPECT_EQ(stringData[3], '\xBF');
	EXPECT_EQ(stringData[4], '\xBD');
	EXPECT_EQ(stringData[5], 'r');
	EXPECT_EQ(stringData[6], '\xD7');
	EXPECT_EQ(stringData[7], '\x93');
	EXPECT_EQ(stringData[8], '\xD7');
	EXPECT_EQ(stringData[9], '\x94');

	EXPECT_EQ(s.getStringLength(), 5);
	EXPECT_EQ(stringData.size(), 10);
}


TEST_F(UTF8StringTest, AStringCanBeInitializedUsingATwoByteAndAFourByteUTF8)
{
	//third character (is ASCII )breaks the 2nd UTF8 character ( 2 byte wide)
	UTF8String s("\xD7\xAA\xf0\x9f\x98\x8b");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], '\xD7');// 1/2
	EXPECT_EQ(stringData[1], '\xAA');// 2/2
	EXPECT_EQ(stringData[2], '\xF0');// 1/4
	EXPECT_EQ(stringData[3], '\x9F');// 2/4
	EXPECT_EQ(stringData[4], '\x98');// 3/4
	EXPECT_EQ(stringData[5], '\x8B');// 4/4

	EXPECT_EQ(s.getStringLength(), 2);
	EXPECT_EQ(stringData.size(), 6);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedUsingAOneByteUTF8)
{
	UTF8String s("\x61\x62\x63\x72\x74\x78");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], 'r');
	EXPECT_EQ(stringData[4], 't');
	EXPECT_EQ(stringData[5], 'x');

	EXPECT_EQ(s.getStringLength(), 6);
	EXPECT_EQ(stringData.size(), 6);
}

TEST_F(UTF8StringTest, AStringCanInitializedUsingACombinationOfAllByteWidths)
{
	//the following string contains
	//- abc in plain ASCII
	//- a two byte UTF8 characters from the hebrew alphabet
	//- a single byte UTF8 character
	//- a four byte UTF8 character
	//- a single byte UTF8 character
	//- a three byte UTF8 character
	//- xyz in plain ASCII
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], '\xD7'); // 1/2
	EXPECT_EQ(stringData[4], '\xAA'); // 2/2
	EXPECT_EQ(stringData[5], '\x78'); // 1/1
	EXPECT_EQ(stringData[6], '\xF0'); // 1/4
	EXPECT_EQ(stringData[7], '\x9F'); // 2/4
	EXPECT_EQ(stringData[8], '\x98'); // 3/4
	EXPECT_EQ(stringData[9], '\x8B'); // 4/4
	EXPECT_EQ(stringData[10], '\x64');// 1/1
	EXPECT_EQ(stringData[11], '\xEA');// 1/3
	EXPECT_EQ(stringData[12], '\xA6');// 2/3
	EXPECT_EQ(stringData[13], '\x85'); // 3/3
	EXPECT_EQ(stringData[14], 'x');
	EXPECT_EQ(stringData[15], 'y');
	EXPECT_EQ(stringData[16], 'z');

	EXPECT_EQ(s.getStringLength(), 11);
	EXPECT_EQ(stringData.size(), 17);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedWithABrokenThreeByteUTF8FollowedByaTwoByteUTF8)
{
	//valid \xEA\xA6\x85 \xD7\xAA
	UTF8String s("\xEA\xA6\xD7\xAA");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], '\xEF');
	EXPECT_EQ(stringData[1], '\xBF');
	EXPECT_EQ(stringData[2], '\xBD');
	EXPECT_EQ(stringData[3], '\xD7');
	EXPECT_EQ(stringData[4], '\xAA');
	
	EXPECT_EQ(s.getStringLength(), 2);
	EXPECT_EQ(stringData.size(), 5);
}

TEST_F(UTF8StringTest, AStringCanBeInitializedWhenTwoOfThreeMultiByteCharactersAreBroken)
{
	//valid \xea\xa6\x8a \xea\xa6\x8d \xea\xa6\x90
	UTF8String s("\xea\xa6\xe6\x8d\xea\xa6\x90");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], '\xEF');
	EXPECT_EQ(stringData[1], '\xBF');
	EXPECT_EQ(stringData[2], '\xBD');
	EXPECT_EQ(stringData[3], '\xEF');
	EXPECT_EQ(stringData[4], '\xBF');
	EXPECT_EQ(stringData[5], '\xBD');
	EXPECT_EQ(stringData[6], '\xEA');
	EXPECT_EQ(stringData[7], '\xA6');
	EXPECT_EQ(stringData[8], '\x90');

	EXPECT_EQ(s.getStringLength(), 3);
	EXPECT_EQ(stringData.size(), 9);
}

TEST_F(UTF8StringTest, AStringWillNotBeConstructedUsingANullPointer)
{
	UTF8String s("");
	const std::string stringData = s.getStringData();
	EXPECT_TRUE(stringData.size() == 0);
	EXPECT_EQ(s.getStringLength(), 0);

	UTF8String s2(s);
	const std::string stringData2 = s2.getStringData();
	EXPECT_TRUE(stringData2.size() == 0);
	EXPECT_EQ(s2.getStringLength(), 0);
}

TEST_F(UTF8StringTest, AStringCanBeSearchedForASCIICharacters)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	int32_t pos_of_c = s.getIndexOf("c");
	EXPECT_EQ(pos_of_c, 2);

	int32_t pos_of_y = s.getIndexOf("y");
	EXPECT_EQ(pos_of_y, 9);
}

TEST_F(UTF8StringTest, AStringCanBeSearchedForASCIICharactersZeroOffsetProvided)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	int32_t pos_of_c = s.getIndexOf("c", 0);
	EXPECT_EQ(pos_of_c, 2);

	int32_t pos_of_y = s.getIndexOf("y", 0);
	EXPECT_EQ(pos_of_y, 9);
}

TEST_F(UTF8StringTest, AStringCanBeSearchedForUTF8Characters)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	const char* utf_character = "\xEA\xA6\x85";

	int32_t pos_of_utf3 = s.getIndexOf(utf_character, 0);
	EXPECT_EQ(pos_of_utf3, 7);
}

TEST_F(UTF8StringTest, AStringDoesNotContainAGivenCharacter)
{
	UTF8String s("abcefgh");

	char character_d = 'd';

	int32_t pos_of_d = s.getIndexOf(&character_d, 0);
	EXPECT_EQ(pos_of_d, -1);
}

TEST_F(UTF8StringTest, AStringIndexOfUsingTheOffsetParameter)
{
	UTF8String s("abcefgh");

	char character_b = 'b';

	int32_t pos_of_b_offset0 = s.getIndexOf(&character_b, 0);
	EXPECT_EQ(pos_of_b_offset0, 1);

	int32_t pos_of_b_offset1 = s.getIndexOf(&character_b, 1);
	EXPECT_EQ(pos_of_b_offset1, 1);

	int32_t pos_of_b_offset2 = s.getIndexOf(&character_b, 2);
	EXPECT_EQ(pos_of_b_offset2, -1);
}

TEST_F(UTF8StringTest, AStringIsDuplicated_ValidString)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");

	UTF8String duplicate(s);

	bool comparison_result = s.equals(duplicate);
	EXPECT_TRUE(comparison_result);
}


TEST_F(UTF8StringTest, AStringIsComparedWithAnIdenticalString)
{
	UTF8String s1("1234567890");
	UTF8String s2("1234567890");
	
	bool comparison = s1.equals(s2);
	EXPECT_TRUE(comparison);
}

TEST_F(UTF8StringTest, AStringIsComparedWithADifferentString)
{
	UTF8String s1("1234567890");
	UTF8String s2("1234567898");

	bool comparison = s1.equals(s2);
	EXPECT_FALSE(comparison);
}

TEST_F(UTF8StringTest, AStringIsComparedWithANullString)
{
	UTF8String s("abc\xD7\xAA\x78\xF0\x9F\x98\x8B\x64\xEA\xA6\x85xyz");
	
	bool comparison = s.equals(NULL);
	EXPECT_FALSE(comparison);
}

/// -----------------

TEST_F(UTF8StringTest, ZeroLengthSubstringFromValidAsciiString)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(0, 0);

	EXPECT_TRUE(substr.getStringData().empty());
	EXPECT_EQ(0, substr.getStringLength());
}

TEST_F(UTF8StringTest, SubstringFromValidAsciiStringStartAtZero)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(0, 3);

	UTF8String expect("012");
	EXPECT_EQ(3, substr.getStringData().size());
	EXPECT_EQ(3, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidAsciiStringMiddle)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(3, 3);

	UTF8String expect("345");
	EXPECT_EQ(3, substr.getStringData().size());
	EXPECT_EQ(3, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidAsciiStringUpToEnd)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(7, 3);

	UTF8String expect("789");
	EXPECT_EQ(3, substr.getStringData().size());
	EXPECT_EQ(3, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidAsciiStringRangeFullRange)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(0, 10);

	UTF8String expect("0123456789");
	EXPECT_EQ(10, substr.getStringData().size());
	EXPECT_EQ(10, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidAsciiStringRangeFullRangeOpenEnd)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(0);

	UTF8String expect("0123456789");
	EXPECT_EQ(10, substr.getStringData().size());
	EXPECT_EQ(10, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidUTF8StringStartAtZero)
{
	UTF8String s(u8"H€lloWorld");

	UTF8String substr = s.substring(0, 5);

	UTF8String expect(u8"H€llo");
	EXPECT_EQ(7, substr.getStringData().size()); // 4*1byte + 1*3byte (€ consumed 3 bytes)
	EXPECT_EQ(5, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidUTF8StringMiddle)
{
	UTF8String s(u8"H€lloWorld");

	UTF8String substr = s.substring(1, 5);

	UTF8String expect(u8"€lloW");
	EXPECT_EQ(7, substr.getStringData().size()); // 4*1byte + 1*3byte (€ consumed 3 bytes)
	EXPECT_EQ(5, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidUTF8StringUpToEnd)
{
	UTF8String s(u8"H€lloWorld");

	UTF8String substr = s.substring(1, 9);

	UTF8String expect(u8"€lloWorld");
	EXPECT_EQ(11, substr.getStringData().size()); // 8*1byte + 1*3byte (€ consumed 3 bytes)
	EXPECT_EQ(9, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidUTF8StringFullRange)
{
	UTF8String s(u8"H€lloWorld");

	UTF8String substr = s.substring(0, 10);

	UTF8String expect(u8"H€lloWorld");
	EXPECT_EQ(12, substr.getStringData().size());// 9*1byte + 1*3byte (€ consumed 3 bytes)
	EXPECT_EQ(10, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidUTF8StringFullRangeOpenEnd)
{
	UTF8String s(u8"H€lloWorld");

	UTF8String substr = s.substring(0);

	UTF8String expect(u8"H€lloWorld");
	EXPECT_EQ(12, substr.getStringData().size());// 9*1byte + 1*3byte (€ consumed 3 bytes)
	EXPECT_EQ(10, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidRange_UTF8Multibyte)
{
	UTF8String s("\xD7\xAA\xD7\x95\xD7\x93\xD7\x94"); // 4 2-byte characters

	UTF8String substr = s.substring(1, 3);

	const char* expect = "\xD7\x95\xD7\x93\xD7\x94";
	EXPECT_EQ(6, substr.getStringData().size());
	EXPECT_EQ(3, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringFromValidRange_UTF8MultibyteASCIIMix)
{
	UTF8String s("\xD7\xAAza\xD7\x95yb\xD7\x93xc\xD7\x94wd"); // 2-byte characters mixed with triplets of ASCII

	UTF8String substr = s.substring(3, 4);

	const char* expect = "\xD7\x95yb\xD7\x93";
	EXPECT_EQ(6, substr.getStringData().size());
	EXPECT_EQ(4, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, SubstringWithInvalidStart)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring((size_t)-1, 7);

	EXPECT_TRUE(substr.getStringLength() == 0);
}

TEST_F(UTF8StringTest, SubstringWithTooLongLength)
{
	UTF8String s("0123456789");

	UTF8String substr = s.substring(1, 12);

	const char* expect = "123456789";
	EXPECT_EQ(9, substr.getStringData().size());
	EXPECT_EQ(9, substr.getStringLength());
	bool comparison_result = substr.equals(expect);
	EXPECT_TRUE(comparison_result);
}

TEST_F(UTF8StringTest, ConcatenateASCIIWithUTFString)
{
	UTF8String s1("abc");
	UTF8String s2("\xD7\x95yb\xD7\x93");

	s1.concatenate(s2);

	const std::string stringData = s1.getStringData();

	EXPECT_EQ(stringData[0], 'a');
	EXPECT_EQ(stringData[1], 'b');
	EXPECT_EQ(stringData[2], 'c');
	EXPECT_EQ(stringData[3], '\xD7');
	EXPECT_EQ(stringData[4], '\x95');
	EXPECT_EQ(stringData[5], 'y');
	EXPECT_EQ(stringData[6], 'b');
	EXPECT_EQ(stringData[7], '\xD7');
	EXPECT_EQ(stringData[8], '\x93');

	EXPECT_EQ(s1.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 9);
}

TEST_F(UTF8StringTest, ConcatenateUTFWithASCIIString)
{
	UTF8String s1("\xD7\xAA\xD7\x95");
	UTF8String s2("test");

	s1.concatenate(s2);

	const std::string stringData = s1.getStringData();

	EXPECT_EQ(stringData[0], '\xD7');
	EXPECT_EQ(stringData[1], '\xAA');
	EXPECT_EQ(stringData[2], '\xD7');
	EXPECT_EQ(stringData[3], '\x95');
	EXPECT_EQ(stringData[4], 't');
	EXPECT_EQ(stringData[5], 'e');
	EXPECT_EQ(stringData[6], 's');
	EXPECT_EQ(stringData[7], 't');

	EXPECT_EQ(s1.getStringLength(), 6);
	EXPECT_EQ(stringData.size(), 8);
}

TEST_F(UTF8StringTest, ConcatenateWithCharPointer)
{
	UTF8String s("part 1 -");
	s.concatenate("part 2");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 'p');
	EXPECT_EQ(stringData[1], 'a');
	EXPECT_EQ(stringData[2], 'r');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], ' ');
	EXPECT_EQ(stringData[5], '1');
	EXPECT_EQ(stringData[6], ' ');
	EXPECT_EQ(stringData[7], '-');
	EXPECT_EQ(stringData[8], 'p');
	EXPECT_EQ(stringData[9], 'a');
	EXPECT_EQ(stringData[10], 'r');
	EXPECT_EQ(stringData[11], 't');
	EXPECT_EQ(stringData[12], ' ');
	EXPECT_EQ(stringData[13], '2');

	EXPECT_EQ(s.getStringLength(), 14);
	EXPECT_EQ(stringData.size(), 14);
}


TEST_F(UTF8StringTest, ConcatenateWithEmptyString)
{
	UTF8String s("test123");
	s.concatenate("");

	const std::string stringData = s.getStringData();

	EXPECT_EQ(stringData[0], 't');
	EXPECT_EQ(stringData[1], 'e');
	EXPECT_EQ(stringData[2], 's');
	EXPECT_EQ(stringData[3], 't');
	EXPECT_EQ(stringData[4], '1');
	EXPECT_EQ(stringData[5], '2');
	EXPECT_EQ(stringData[6], '3');

	EXPECT_EQ(s.getStringLength(), 7);
	EXPECT_EQ(stringData.size(), 7);
}

TEST_F(UTF8StringTest, EmptyString)
{
	UTF8String s("");
	EXPECT_TRUE(s.empty());
}

TEST_F(UTF8StringTest, NotEmptyString)
{
	UTF8String s("Hello World");
	EXPECT_FALSE(s.empty());
}

TEST_F(UTF8StringTest, SplitEmptyString)
{
	UTF8String s("");
	std::vector<UTF8String> parts = s.split(' ');
	EXPECT_TRUE(parts.empty());
}

TEST_F(UTF8StringTest, SplitAsciiStringDelimNotExists)
{
	UTF8String s("HelloWorld");
	std::vector<UTF8String> parts = s.split(' ');
	EXPECT_EQ(1, parts.size());
	EXPECT_TRUE(parts.at(0).equals(s));
}

TEST_F(UTF8StringTest, SplitAsciiString)
{
	UTF8String s("Hello World");
	std::vector<UTF8String> parts = s.split(' ');
	EXPECT_EQ(2, parts.size());
	EXPECT_TRUE(parts.at(0).equals("Hello"));
	EXPECT_TRUE(parts.at(1).equals("World"));
}

TEST_F(UTF8StringTest, SplitAsciiStringMultipleDelimExists)
{
	UTF8String s("One,Two,Three");
	std::vector<UTF8String> parts = s.split(',');
	EXPECT_EQ(3, parts.size());
	EXPECT_TRUE(parts.at(0).equals("One"));
	EXPECT_TRUE(parts.at(1).equals("Two"));
	EXPECT_TRUE(parts.at(2).equals("Three"));
}

TEST_F(UTF8StringTest, SplitAsciiStringMultipleDelimExistsWithEmptyParts)
{
	UTF8String s("One,Two,,Four,Five,,");
	std::vector<UTF8String> parts = s.split(',');
	EXPECT_EQ(6, parts.size());
	EXPECT_TRUE(parts.at(0).equals("One"));
	EXPECT_TRUE(parts.at(1).equals("Two"));
	EXPECT_TRUE(parts.at(2).empty());
	EXPECT_TRUE(parts.at(3).equals("Four"));
	EXPECT_TRUE(parts.at(4).equals("Five"));
	EXPECT_TRUE(parts.at(5).empty());
}

TEST_F(UTF8StringTest, SplitUtf8StringDelimNotExists)
{
	UTF8String s(u8"H€lloWorld");
	std::vector<UTF8String> parts = s.split(' ');
	EXPECT_EQ(1, parts.size());
	EXPECT_TRUE(parts.at(0).equals(s));
}

TEST_F(UTF8StringTest, SplitUtf8String)
{
	UTF8String s(u8"H€llo World");
	std::vector<UTF8String> parts = s.split(' ');
	EXPECT_EQ(2, parts.size());
	EXPECT_TRUE(parts.at(0).equals(u8"H€llo"));
	EXPECT_TRUE(parts.at(1).equals(u8"World"));
}

TEST_F(UTF8StringTest, SplitUtf8StringMultipleDelimExists)
{
	UTF8String s(u8"On€,Two,Thr€e");
	std::vector<UTF8String> parts = s.split(',');
	EXPECT_EQ(3, parts.size());
	EXPECT_TRUE(parts.at(0).equals(u8"On€"));
	EXPECT_TRUE(parts.at(1).equals(u8"Two"));
	EXPECT_TRUE(parts.at(2).equals(u8"Thr€e"));
}

TEST_F(UTF8StringTest, SplitUtf8StringMultipleDelimExistsWithEmptyParts)
{
	UTF8String s(u8"On€,Two,,Four,Fiv€,,");
	std::vector<UTF8String> parts = s.split(',');
	EXPECT_EQ(6, parts.size());
	EXPECT_TRUE(parts.at(0).equals(u8"On€"));
	EXPECT_TRUE(parts.at(1).equals(u8"Two"));
	EXPECT_TRUE(parts.at(2).empty());
	EXPECT_TRUE(parts.at(3).equals(u8"Four"));
	EXPECT_TRUE(parts.at(4).equals(u8"Fiv€"));
	EXPECT_TRUE(parts.at(5).empty());
}
