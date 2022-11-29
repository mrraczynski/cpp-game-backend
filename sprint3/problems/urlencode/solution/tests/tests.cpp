#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    std::string s1(1, char(31));
    std::string s2(1, char(128));
    EXPECT_EQ(UrlEncode(""s), ""s);
    EXPECT_EQ(UrlEncode("hello"s), "hello"s);
    EXPECT_EQ(UrlEncode("hello!"s), "hello%21"s);
    EXPECT_EQ(UrlEncode("hello !"s), "hello+%21"s);
    EXPECT_EQ(UrlEncode(s1 + "hello !"s + s2), "%1Fhello+%21%80"s);
}

/* Напишите остальные тесты самостоятельно */
