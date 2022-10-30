#include "request_handler.h"
#include <charconv>

namespace http_handler {
	std::vector<std::string_view> RequestHandler::SplitRequest(const std::string_view target)
	{
        std::vector<std::string_view> target_vec;
        std::string_view delim("/");
        size_t prev = 0, next = 0;
        size_t delta = delim.length();

        while ((next = target.find(delim, prev)) != std::string::npos) {
            target_vec.push_back(target.substr(prev, next - prev));
            prev = next + delta;
        }
        target_vec.push_back(target.substr(prev));

        return target_vec;
	}

    bool RequestHandler::IsApiRequest(const std::vector<std::string_view>& target_vec)
    {
        return target_vec[1].compare("api") == 0;
    }

    bool RequestHandler::IsGoodRequest(const std::vector<std::string_view>& target_vec)
    {
        if (target_vec.size() >= 4)
        {
            return IsApiRequest(target_vec) &&
                target_vec[2].compare("v1") == 0 &&
                target_vec[3].compare("maps") == 0;
        }
        else
        {
            return false;
        }
    }

    bool RequestHandler::HasMapID(const std::vector<std::string_view>& target_vec)
    {
        return IsGoodRequest(target_vec) && target_vec.size() > 4;
    }

    const model::Map* RequestHandler::FindMapID(const std::vector<std::string_view>& target_vec)
    {
        const model::Map* map_id = game_.FindMap(model::Map::Id(std::string(target_vec[4])));
        if (map_id != nullptr)
        {
            return map_id;
        }
        return nullptr;
    }

    std::string RequestHandler::UrlDecode(const std::string_view& value)
    {
        std::string result;
        result.reserve(value.size());

        auto x = value.size();

        for (std::size_t i = 0; i < value.size(); ++i)
        {
            auto ch = value[i];

            if (ch == '%' && (i + 2) < value.size())
            {
                auto hex = value.substr(i + 1, 2);                
                char dec;
                std::from_chars(hex.data(), hex.data() + hex.size(), dec, 16);
                result.push_back(dec);
                i += 2;
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

    // Возвращает true, если каталог p содержится внутри base_path.
    bool RequestHandler::IsSubPath(fs::path path, fs::path base) {
        // Приводим оба пути к каноничному виду (без . и ..)
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);

        // Проверяем, что все компоненты base содержатся внутри path
        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }

    std::pair<bool, std::string_view> RequestHandler::GetFileContentType(const std::string& decoded_path)
    {
        if (decoded_path[decoded_path.size() - 1] == '/' || decoded_path[decoded_path.size() - 1] == '\\')
        {
            return std::pair<bool, const std::string_view&>(false, ContentType::FOLDER);
        }
        for (int pos = decoded_path.size() - 1; pos >= 0; pos--)
        {
            if (decoded_path[pos] == '.')
            {
                if (std::strcmp(&decoded_path[pos], ".html") == 0 || std::strcmp(&decoded_path[pos], ".htm") == 0)
                {                    
                    return std::pair<bool, const std::string_view&>(true, ContentType::TEXT_HTML);
                }               
                else if (std::strcmp(&decoded_path[pos], ".css") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::TEXT_CSS);
                }
                else if (std::strcmp(&decoded_path[pos], ".txt") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::TEXT_PLAIN);
                }
                else if (std::strcmp(&decoded_path[pos], ".js") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::TEXT_JAVASCRIPT);
                }
                else if (std::strcmp(&decoded_path[pos], ".json") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::APPLICATION_JSON);
                }
                else if (std::strcmp(&decoded_path[pos], ".xml") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::APPLICATION_XML);
                }
                else if (std::strcmp(&decoded_path[pos], ".png") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_PNG);
                }
                else if (std::strcmp(&decoded_path[pos], ".jpg") == 0 || std::strcmp(&decoded_path[pos], ".jpeg") == 0 || std::strcmp(&decoded_path[pos], ".jpe") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_JPEG);
                }
                else if (std::strcmp(&decoded_path[pos], ".gif") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_GIF);
                }
                else if (std::strcmp(&decoded_path[pos], ".bmp") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_BMP);
                }
                else if (std::strcmp(&decoded_path[pos], ".ico") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_ICO);
                }
                else if (std::strcmp(&decoded_path[pos], ".tiff") == 0 || std::strcmp(&decoded_path[pos], ".tif") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_TIFF);
                }
                else if (std::strcmp(&decoded_path[pos], ".svg") == 0 || std::strcmp(&decoded_path[pos], ".svgz") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::IMAGE_SVG);
                }
                else if (std::strcmp(&decoded_path[pos], ".mp3") == 0)
                {
                    return std::pair<bool, const std::string_view&>(true, ContentType::AUDIO_MPEG);
                }

            }
            else if (decoded_path[pos] == '/' || decoded_path[pos] == '\\')
            {
                return std::pair<bool, const std::string_view&>(false, ContentType::FOLDER);
            }
        }
        return std::pair<bool, const std::string_view&>(false, ContentType::FOLDER);
    }

}  // namespace http_handler
