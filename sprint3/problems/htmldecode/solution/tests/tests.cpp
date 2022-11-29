#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

using namespace std::literals;

TEST_CASE("Text without mnemonics", "[HtmlDecode]") {
    CHECK(HtmlDecode(""sv) == ""s);
    CHECK(HtmlDecode("hello"sv) == "hello"s);
    CHECK(HtmlDecode("hello &amp"sv) == "hello &"s);
    CHECK(HtmlDecode("hello &AMP"sv) == "hello &"s);
    CHECK(HtmlDecode("hello &AmP"sv) == "hello &AmP"s);
    CHECK(HtmlDecode("&lt hel&quotlo &AMP"sv) == "< hel\"lo &"s);
    CHECK(HtmlDecode("&lt hel&quolo &AMP"sv) == "< hel&quolo &"s);
    CHECK(HtmlDecode("&lt; hello &AMP"sv) == "< hello &"s);
}

// Напишите недостающие тесты самостоятельно
