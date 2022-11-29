#include "htmldecode.h"
#include <map>

using namespace std::literals;

std::map<std::string, std::string> upper_mnemonics = {
    {"&lt"s, "<"s},
    {"&gt"s, ">"s},
    {"&amp"s, "&"s},
    {"&apos"s, "'"s},
    {"&quot"s, "\""s}
};

std::map<std::string, std::string> lower_mnemonics = {
    {"&LT"s, "<"s},
    {"&GT"s, ">"s},
    {"&AMP"s, "&"s},
    {"&APOS"s, "'"s},
    {"&QUOT"s, "\""s}
};

std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count{};
    for (std::string::size_type pos{};
        inout.npos != (pos = inout.find(what.data(), pos, what.length()));
        pos += with.length(), ++count) {
        inout.replace(pos, what.length(), with.data(), with.length());
        if (inout[pos + with.length()] == ';')
        {
            inout.erase(pos + with.length(), 1);
        }
    }
    return count;
}

std::string HtmlDecode(std::string_view str) {
    
    std::string result(str);

    for (auto& up_mnem : upper_mnemonics)
    {
        replace_all(result, up_mnem.first, up_mnem.second);
    }

    for (auto& low_mnem : lower_mnemonics)
    {
        replace_all(result, low_mnem.first, low_mnem.second);
    }

    return result;
}
