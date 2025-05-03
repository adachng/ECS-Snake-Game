#include <gtest/gtest.h>

namespace
{
    enum MapSlotState : uint8_t
    {
        EMPTY = 0b000U,
        SNAKE_HEAD = 0b001U,
        SNAKE_BODY = 0b010U,
        APPLE = 0b100U,
        ENUM_END = 0b111U,
    }; // enum MapSlotState

    TEST(CPPEnumTest, EnumSyntax)
    {
        MapSlotState x = EMPTY;
        EXPECT_EQ(x, 0U);

        uint8_t y = 0b101U;
        EXPECT_FALSE(y & EMPTY);
        EXPECT_TRUE(y & SNAKE_HEAD);
        EXPECT_FALSE(y & SNAKE_BODY);
        EXPECT_TRUE(y & APPLE);

        MapSlotState z = static_cast<MapSlotState>(y);
        EXPECT_FALSE(z & EMPTY);
        EXPECT_TRUE(z & SNAKE_HEAD);
        EXPECT_FALSE(z & SNAKE_BODY);
        EXPECT_TRUE(z & APPLE);
    }
} // namespace
