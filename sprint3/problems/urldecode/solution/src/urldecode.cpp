#include "urldecode.h"

#include <charconv>
#include <stdexcept>

bool isSequenceValid(const std::string_view& str)
{
    if (str.size() < 2)
    {
        return false;
    }
    std::string alph = "0123456789ABCDEF";    
    return alph.find(str[0]) != std::string::npos && alph.find(str[1]) != std::string::npos;
}

std::string UrlDecode(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    auto x = str.size();

    for (std::size_t i = 0; i < str.size(); ++i)
    {
        auto ch = str[i];

        if (ch == '%' && (i + 2) < str.size())
        {
            std::string_view hex = str.substr(i + 1, 2);
            if (isSequenceValid(hex))
            {
                char dec;
                auto res = std::from_chars(hex.data(), hex.data() + hex.size(), dec, 16);
                if (res.ec == std::errc())
                {
                    result.push_back(dec);
                    i += 2;
                }
                else
                {
                    result.push_back(ch);
                }
            }
            else
            {
                result.push_back(ch);
            }
        }
        else if (ch == '+')
        {
            result.push_back(' ');
        }
        else
        {
            result.push_back(ch);
        }
    }
    return result;
}
