#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
    using namespace std::literals;

    BOOST_TEST(UrlDecode(""sv) == ""s);
    BOOST_TEST(UrlDecode("Some-string-without-sequence"sv) == "Some-string-without-sequence"s);
    BOOST_TEST(UrlDecode("Some+string+with+spaces"sv) == "Some string with spaces"s);
    BOOST_TEST(UrlDecode("Some+%21+string+%23+with+%24+sequence"sv) == "Some ! string # with $ sequence"s);
    BOOST_TEST(UrlDecode("Some+%21+string+%23+with+%24+invalid+%88+value"sv) == "Some ! string # with $ invalid %88 value"s);
    BOOST_TEST(UrlDecode("Some+%21+string+%23+with+%24+incomplete+%2+value"sv) == "Some ! string # with $ incomplete %2 value"s);
    // Напишите остальные тесты для функции UrlDecode самостоятельно
}