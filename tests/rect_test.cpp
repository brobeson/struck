#define BOOST_TEST_MODULE "Rectangle Test"
#include <boost/test/included/unit_test.hpp>
#include "Rect.h"

BOOST_AUTO_TEST_SUITE(struck)
BOOST_AUTO_TEST_SUITE(rectangle)

BOOST_AUTO_TEST_CASE(diagonal_length_with_0_width)
{
    Rect<int> a(0, 0, 0, 10);
    constexpr int expected_length = 10;
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_0_height)
{
    Rect<int> a(0, 0, 10, 0);
    constexpr int expected_length = 10;
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_0_area)
{
    Rect<int> a(0, 0, 0, 0);
    constexpr int expected_length = 0;
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_negative_width)
{
    Rect<int> a(0, 0, -10, 10);
    constexpr int expected_length = 14; // floor of sqrt(200)
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_negative_height)
{
    Rect<int> a(0, 0, 10, -10);
    constexpr int expected_length = 14; // floor of sqrt(200)
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_negative_height_and_width)
{
    Rect<int> a(0, 0, -10, -10);
    constexpr int expected_length = 14; // floor of sqrt(200)
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_CASE(diagonal_length_with_positive_height_and_width)
{
    Rect<int> a(0, 0, 10, 10);
    constexpr int expected_length = 14; // floor of sqrt(200)
    BOOST_CHECK_EQUAL(expected_length, diagonal_length<int>(a));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
