/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <qtils/hex.hpp>

using namespace std::string_literals;

/**
 * @test Hex formatting using fmtlib
 *
 * @details
 * These tests verify formatting behavior of the qtils::Hex wrapper
 * under various conditions and format specifiers.
 * The supported format specifiers are:
 *   - {}      : abbreviated lowercase hex, with ellipsis in the middle
 *   - {:x}    : same as default, lowercase abbreviated
 *   - {:xx}   : full lowercase hex
 *   - {:X}    : abbreviated uppercase hex
 *   - {:XX}   : full uppercase hex
 *   - {:0}    : abbreviated lowercase with "0x" prefix
 *   - {:0x}   : same as {:0}
 *   - {:0xx}  : full lowercase with "0x" prefix
 *   - {:0X}   : abbreviated uppercase with "0x" prefix
 *   - {:0XX}  : full uppercase with "0x" prefix
 */

/**
 * @given A 5-byte buffer
 * @when Formatted with various specifiers
 * @then The output is abbreviated (head+tail with ellipsis), or full in xx/XX
 */
TEST(HexTest, FormatLen5) {
  uint8_t bytes[] = {0x01, 0x02, 0x00, 0x0a, 0x0b};
  auto hex = qtils::Hex{bytes};

  EXPECT_EQ(fmt::format("{}", hex),       "0102…0a0b"s);
  EXPECT_EQ(fmt::format("{:x}", hex),     "0102…0a0b"s);
  EXPECT_EQ(fmt::format("{:xx}", hex),    "0102000a0b"s);
  EXPECT_EQ(fmt::format("{:X}", hex),     "0102…0A0B"s);
  EXPECT_EQ(fmt::format("{:XX}", hex),    "0102000A0B"s);
  EXPECT_EQ(fmt::format("{:0}", hex),   "0x0102…0a0b"s);
  EXPECT_EQ(fmt::format("{:0x}", hex),  "0x0102…0a0b"s);
  EXPECT_EQ(fmt::format("{:0xx}", hex), "0x0102000a0b"s);
  EXPECT_EQ(fmt::format("{:0X}", hex),  "0x0102…0A0B"s);
  EXPECT_EQ(fmt::format("{:0XX}", hex), "0x0102000A0B"s);
}

/**
 * @given A 4-byte buffer (length == kHead + kTail + kSmall)
 * @when Formatted with various specifiers
 * @then The output is always full, no abbreviation is applied
 */
TEST(HexTest, FormatLen4) {
  uint8_t bytes[] = {0x01, 0x02, 0x0a, 0x0b};
  auto hex = qtils::Hex{bytes};

  EXPECT_EQ(fmt::format("{}", hex),       "01020a0b"s);
  EXPECT_EQ(fmt::format("{:x}", hex),     "01020a0b"s);
  EXPECT_EQ(fmt::format("{:xx}", hex),    "01020a0b"s);
  EXPECT_EQ(fmt::format("{:X}", hex),     "01020A0B"s);
  EXPECT_EQ(fmt::format("{:XX}", hex),    "01020A0B"s);
  EXPECT_EQ(fmt::format("{:0}", hex),   "0x01020a0b"s);
  EXPECT_EQ(fmt::format("{:0x}", hex),  "0x01020a0b"s);
  EXPECT_EQ(fmt::format("{:0xx}", hex), "0x01020a0b"s);
  EXPECT_EQ(fmt::format("{:0X}", hex),  "0x01020A0B"s);
  EXPECT_EQ(fmt::format("{:0XX}", hex), "0x01020A0B"s);
}

/**
 * @given A 3-byte buffer (less than an abbreviation threshold)
 * @when Formatted with any specifier
 * @then The output is always full
 */
TEST(HexTest, FormatLen3) {
  uint8_t bytes[] = {0x01, 0x00, 0xff};
  auto hex = qtils::Hex{bytes};

  EXPECT_EQ(fmt::format("{}", hex),       "0100ff"s);
  EXPECT_EQ(fmt::format("{:x}", hex),     "0100ff"s);
  EXPECT_EQ(fmt::format("{:xx}", hex),    "0100ff"s);
  EXPECT_EQ(fmt::format("{:X}", hex),     "0100FF"s);
  EXPECT_EQ(fmt::format("{:XX}", hex),    "0100FF"s);
  EXPECT_EQ(fmt::format("{:0}", hex),   "0x0100ff"s);
  EXPECT_EQ(fmt::format("{:0x}", hex),  "0x0100ff"s);
  EXPECT_EQ(fmt::format("{:0xx}", hex), "0x0100ff"s);
  EXPECT_EQ(fmt::format("{:0X}", hex),  "0x0100FF"s);
  EXPECT_EQ(fmt::format("{:0XX}", hex), "0x0100FF"s);
}

/**
 * @given An empty buffer
 * @when Formatted with any specifier
 * @then Outputs "<empty>" or "0x" depending on a prefix flag
 */
TEST(HexTest, FormatEmpty) {
  std::vector<uint8_t> bytes;
  auto hex = qtils::Hex{bytes};

  EXPECT_EQ(fmt::format("{}", hex),       "<empty>"s);
  EXPECT_EQ(fmt::format("{:x}", hex),     "<empty>"s);
  EXPECT_EQ(fmt::format("{:xx}", hex),    "<empty>"s);
  EXPECT_EQ(fmt::format("{:X}", hex),     "<empty>"s);
  EXPECT_EQ(fmt::format("{:XX}", hex),    "<empty>"s);
  EXPECT_EQ(fmt::format("{:0}", hex),   "0x"s);
  EXPECT_EQ(fmt::format("{:0x}", hex),  "0x"s);
  EXPECT_EQ(fmt::format("{:0xx}", hex), "0x"s);
  EXPECT_EQ(fmt::format("{:0X}", hex),  "0x"s);
  EXPECT_EQ(fmt::format("{:0XX}", hex), "0x"s);
}
