/**
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

#include "core/util/DefaultLogger.h"

#include <gtest/gtest.h>

using namespace core::util;

class DefaultLoggerTest : public testing::Test
{

};

TEST_F(DefaultLoggerTest, defaultLoggerWithVerboseOutputWritesErrorLevelMessages)
{
	// given
	DefaultLogger logger(true);

	// then 
	ASSERT_TRUE(logger.isErrorEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithVerboseOutputWritesWarnLevelMessages)
{
	// given
	DefaultLogger logger(true);

	// then 
	ASSERT_TRUE(logger.isWarningEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithVerboseOutputWritesInfoLevelMessages)
{
	// given
	DefaultLogger logger(true);

	// then 
	ASSERT_TRUE(logger.isInfoEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithVerboseOutputWritesDebugLevelMessages)
{
	// given
	DefaultLogger logger(true);

	// then 
	ASSERT_TRUE(logger.isDebugEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithoutVerboseOutputWritesErrorLevelMessages)
{
	// given
	DefaultLogger logger(false);

	// then 
	ASSERT_TRUE(logger.isErrorEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithoutVerboseOutputWritesWarnLevelMessages)
{
	// given
	DefaultLogger logger(false);

	// then 
	ASSERT_TRUE(logger.isWarningEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithoutVerboseOutputWritesInfoLevelMessages)
{
	// given
	DefaultLogger logger(false);

	// then 
	ASSERT_FALSE(logger.isInfoEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerWithoutVerboseOutputWritesDebugLevelMessages)
{
	// given
	DefaultLogger logger(false);

	// then 
	ASSERT_FALSE(logger.isDebugEnabled());
}

TEST_F(DefaultLoggerTest, defaultLoggerPrintOutInteger)
{
	// given
	std::ostringstream oss;
	DefaultLogger logger(oss, true);

	// then
	uint32_t i = 7;
	logger.debug("Some string with '%u' as uint32_t", i);
	auto found = oss.str().find("DEBUG");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("Some string with '7' as uint32_t\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(DefaultLoggerTest, defaultLoggerPrintOutString)
{
	// given
	std::ostringstream oss;
	DefaultLogger logger(oss, true);

	// then
	std::string str("World");
	logger.info("Hello %s!!!", str.c_str());
	auto found = oss.str().find("INFO");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("Hello World!!!\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(DefaultLoggerTest, defaultLoggerPrintOutDouble)
{
	// given
	std::ostringstream oss;
	DefaultLogger logger(oss, true);

	// then
	double pi = 3.14159265358979323846;
	logger.warning("Pi=%.4f", pi);
	auto found = oss.str().find("WARN");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("Pi=3.1416\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(DefaultLoggerTest, defaultLoggerPrintOutMultiple)
{
	// given
	std::ostringstream oss;
	DefaultLogger logger(oss, true);

	// then
	double pi = 3.14159265358979323846;
	int32_t a = -2;
	std::string b = "three";
	logger.error("Pi=%.4f, a=%d, b='%s'", pi, a, b.c_str());
	auto found = oss.str().find("ERROR");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("Pi=3.1416, a=-2, b='three'\n");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}

TEST_F(DefaultLoggerTest, defaultLoggerPrintOutAVeryLongString)
{
	// given
	std::ostringstream oss;
	DefaultLogger logger(oss, true);

	// then
	std::string longText = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. "
		"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat."
		"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi."
		"Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat."
		"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis."
		"At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum.sanctus sea sed takimata ut vero voluptua.est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat."
		"Consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus."
		"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet."
		"Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat."
		"Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi."
		"Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo";
	logger.debug("This will be a very long text: '%s'", longText.c_str());
	auto found = oss.str().find("DEBUG");
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("This will be a very long text: 'Lorem ipsum dolor "); // check the first words
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
	found = oss.str().find("nisl ut aliquip ex ea commodo'\n"); // check the last words
	ASSERT_TRUE(found != std::string::npos) << "Unexpected log statement: " << oss.str() << std::endl;
}