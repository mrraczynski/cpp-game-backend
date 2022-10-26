#include "request_handler.h"

namespace http_handler {
	std::vector<std::string_view> RequestHandler::SplitRequest(const std::string_view target)
	{
        std::vector<std::string_view> target_vec;
        std::string_view delim("/");
        size_t prev = 0;
        size_t next;
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
        return IsApiRequest(target_vec) && target_vec[2].compare("v1") == 0 && target_vec[3].compare("maps") == 0;
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

}  // namespace http_handler
