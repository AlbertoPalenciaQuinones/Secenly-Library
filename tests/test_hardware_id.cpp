#include <gtest/gtest.h>

#include "library/hardware_manager.h"

TEST(HardwareIdentifierTest, InitializeReturnsTrue) {
    HardwareManager hw;

    EXPECT_TRUE(hw.initialize());
}

TEST(HardwareIdentifierTest, HwidIsNotEmpty) {
    HardwareManager hw;

    ASSERT_TRUE(hw.initialize());

    EXPECT_FALSE(hw.getHwid().empty());
}

TEST(HardwareManagerTest, HwidIsStable) {
    HardwareManager hw1, hw2;

    ASSERT_TRUE(hw1.initialize());
    ASSERT_TRUE(hw2.initialize());

    EXPECT_EQ(hw1.getHwid(), hw2.getHwid());
}

TEST(HardwareManagerTest, HwidHasNoTrailingNewline) {
    HardwareManager hw;

    ASSERT_TRUE(hw.initialize());

    std::string id = hw.getHwid();

    EXPECT_TRUE(id.empty() || id.back() != '\n');
}


