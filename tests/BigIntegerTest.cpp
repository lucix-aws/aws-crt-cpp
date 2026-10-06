/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/BigInteger.h>
#include <aws/testing/aws_test_harness.h>

#include <utility>

using namespace Aws::Crt;

static const char *s_hugeInteger = "-123456789012345678901234567890123456789012345678901234567890";

static int s_BigIntegerRoundTripTest(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        ApiHandle apiHandle(allocator);

        auto huge = BigInteger::FromString(aws_byte_cursor_from_c_str(s_hugeInteger), allocator);
        ASSERT_TRUE(huge.has_value());
        ASSERT_TRUE(huge->ToString() == s_hugeInteger);

        /* Leading zeros are dropped and zero is never negative. */
        auto zero = BigInteger::FromString(aws_byte_cursor_from_c_str("-000"), allocator);
        ASSERT_TRUE(zero.has_value());
        ASSERT_TRUE(zero->ToString() == "0");

        auto fromInt = BigInteger::FromInt64(INT64_MIN, allocator);
        ASSERT_TRUE(fromInt.has_value());
        ASSERT_TRUE(fromInt->ToString() == "-9223372036854775808");
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(BigIntegerRoundTripTest, s_BigIntegerRoundTripTest)

static int s_BigIntegerInvalidInputTest(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        ApiHandle apiHandle(allocator);

        auto bad = BigInteger::FromString(aws_byte_cursor_from_c_str("12x"), allocator);
        ASSERT_FALSE(bad.has_value());
        ASSERT_INT_EQUALS(AWS_ERROR_INVALID_ARGUMENT, LastError());

        auto empty = BigInteger::FromString(aws_byte_cursor_from_c_str(""), allocator);
        ASSERT_FALSE(empty.has_value());
        ASSERT_INT_EQUALS(AWS_ERROR_INVALID_ARGUMENT, LastError());

        auto padded = BigInteger::FromString(aws_byte_cursor_from_c_str(" 1"), allocator);
        ASSERT_FALSE(padded.has_value());
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(BigIntegerInvalidInputTest, s_BigIntegerInvalidInputTest)

static int s_BigIntegerEqualityTest(struct aws_allocator *allocator, void *ctx)
{
    (void)ctx;
    {
        ApiHandle apiHandle(allocator);

        auto a = BigInteger::FromString(aws_byte_cursor_from_c_str(s_hugeInteger), allocator);
        auto b = BigInteger::FromString(aws_byte_cursor_from_c_str(s_hugeInteger + 1), allocator);
        auto aAgain = BigInteger::FromString(
            aws_byte_cursor_from_c_str("-000123456789012345678901234567890123456789012345678901234567890"), allocator);
        ASSERT_TRUE(a.has_value() && b.has_value() && aAgain.has_value());

        ASSERT_TRUE(*a == *aAgain);
        ASSERT_TRUE(*a != *b);

        auto zero = BigInteger::FromInt64(0, allocator);
        auto negativeZero = BigInteger::FromString(aws_byte_cursor_from_c_str("-0"), allocator);
        ASSERT_TRUE(zero.has_value() && negativeZero.has_value());
        ASSERT_TRUE(*zero == *negativeZero);
        ASSERT_TRUE(*zero != *a);

        BigInteger moved = std::move(*a);
        ASSERT_TRUE(moved == *aAgain);
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE(BigIntegerEqualityTest, s_BigIntegerEqualityTest)
