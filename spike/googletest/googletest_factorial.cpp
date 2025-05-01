#include <gtest/gtest.h>

namespace
{
    int factorial(int n); // Returns the factorial of n

    // Tests factorial of 0.
    TEST(FactorialTest, HandlesZeroInput)
    {
        EXPECT_EQ(factorial(0), 1);
    }

    // Tests factorial of positive numbers.
    TEST(FactorialTest, HandlesPositiveInput)
    {
        EXPECT_EQ(factorial(1), 1);
        EXPECT_EQ(factorial(2), 2);
        EXPECT_EQ(factorial(3), 6);
        EXPECT_EQ(factorial(8), 40320);
        EXPECT_EQ(factorial(9), 362880);
    }

    TEST(FactorialTest, HandlesNegativeInput)
    {
        EXPECT_EQ(factorial(-1), -1);
        EXPECT_EQ(factorial(-2), 2);
        EXPECT_EQ(factorial(-3), -6);
        EXPECT_EQ(factorial(-8), 40320);
        EXPECT_EQ(factorial(-9), -362880);
    }

    int factorial(int n)
    {
        if (n < 0)
        {
            n *= -1;
            return (n % 2 == 0) ? factorial(n) : -1 * factorial(n);
        }
        else if (n <= 1)
            return 1;
        else
            return n * factorial(n - 1);
    }
} // namespace
