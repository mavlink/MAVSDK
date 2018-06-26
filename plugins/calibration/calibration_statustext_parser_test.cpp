#include "calibration_statustext_parser.h"
#include <gtest/gtest.h>

using namespace dronecore;

TEST(CalibrationStatustextParser, Validity)
{
    CalibrationStatustextParser parser;
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
    EXPECT_FALSE(parser.parse("something random"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
    parser.reset();

    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
    EXPECT_TRUE(parser.parse("[cal] calibration started: 2 gyro"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::STARTED);
    EXPECT_TRUE(parser.parse("[cal] progress <75>"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::PROGRESS);
    EXPECT_FLOAT_EQ(parser.get_progress(), 0.75f);
    parser.reset();
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
}

TEST(CalibrationStatustextParser, Started)
{
    CalibrationStatustextParser parser;

    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
    EXPECT_TRUE(parser.parse("[cal] calibration started: 2 gyro"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::STARTED);
    parser.reset();
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::NONE);
}

TEST(CalibrationStatustextParser, Progress)
{
    CalibrationStatustextParser parser;

    EXPECT_TRUE(parser.parse("[cal] progress <10>"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::PROGRESS);
    EXPECT_FLOAT_EQ(parser.get_progress(), 0.10);

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] right side calibration: progress <78>"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::PROGRESS);
    EXPECT_FLOAT_EQ(parser.get_progress(), 0.78);

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] up side calibration: progress <45>"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::PROGRESS);
    EXPECT_FLOAT_EQ(parser.get_progress(), 0.45);
}

TEST(CalibrationStatustextParser, Failed)
{
    CalibrationStatustextParser parser;

    EXPECT_TRUE(parser.parse("[cal] calibration failed: Not happy jam"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::FAILED);
    EXPECT_STREQ(parser.get_failed_message().c_str(), "Not happy jam");
}

TEST(CalibrationStatustextParser, Cancelled)
{
    CalibrationStatustextParser parser;

    EXPECT_TRUE(parser.parse("[cal] calibration cancelled"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::CANCELLED);
}

TEST(CalibrationStatustextParser, Instructions)
{
    CalibrationStatustextParser parser;

    EXPECT_TRUE(parser.parse("[cal] top orientation detected"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "top orientation detected");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] bottom side done, rotate to a different side"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "bottom side done, rotate to a different side");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] right side already completed, rotate to a different side"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(),
                 "right side already completed, rotate to a different side");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] down side already completed"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "down side already completed");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] detected rest position, hold still..."));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "detected rest position, hold still...");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] Hold still, measuring down side"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "Hold still, measuring down side");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] hold vehicle still on a pending side"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "hold vehicle still on a pending side");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] pending: back front left right up down"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "pending: back front left right up down");

    parser.reset();
    EXPECT_TRUE(parser.parse("[cal] down side result: [  0.0089  -0.4756 -10.43 ]"));
    EXPECT_EQ(parser.get_status(), CalibrationStatustextParser::Status::INSTRUCTION);
    EXPECT_STREQ(parser.get_instruction().c_str(), "down side result: [  0.0089  -0.4756 -10.43 ]");
}
