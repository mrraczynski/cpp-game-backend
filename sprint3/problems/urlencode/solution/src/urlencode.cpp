#include "urlencode.h"

#include <charconv>
#include <stdexcept>
#include <sstream>
#include <math.h>

bool isSequenceValid(char c)
{
    if (c < 32 || c >= 128)
    {
        return true;
    }
    std::string alph = "!#$&'()*+,/:;=?@[]";
    return alph.find(c) != std::string::npos;
}

std::string UrlEncode(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    auto x = str.size();

    for (std::size_t i = 0; i < str.size(); ++i)
    {
        auto ch = str[i];

        if (isSequenceValid(ch))
        {
            std::stringstream ss;
            ss << '%' << std::hex << std::uppercase << std::abs((int)ch);
            result += ss.str();
        }
        else if (ch == ' ')
        {
            result.push_back('+');
        }
        else
        {
            result.push_back(ch);
        }
    }
    return result;
}
