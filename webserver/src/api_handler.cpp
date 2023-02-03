#include "api_handler.h"
#include <charconv>

namespace http_handler {
    std::vector<std::string_view> ApiHandler::SplitRequest(const std::string_view target)
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

    bool ApiHandler::IsApiRequest(const std::vector<std::string>& target_vec)
    {
        return target_vec.size() > 1 ? target_vec[1].compare("api") == 0 : false;
    }

    bool ApiHandler::IsMapsRequest(const std::vector<std::string>& target_vec)
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

    bool ApiHandler::IsGameRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() >= 4)
        {
            return IsApiRequest(target_vec) &&
                target_vec[2].compare("v1") == 0 &&
                target_vec[3].compare("game") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsPlayerActionRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 6)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("player") == 0 &&
                target_vec[5].compare("action") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsTimeTickRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 5)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("tick") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsRecordsRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 5)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("records") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsGameJoinRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 5)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("join") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsGamePlayersRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 5)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("players") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::IsGameStateRequest(const std::vector<std::string>& target_vec)
    {
        if (target_vec.size() == 5)
        {
            return IsGameRequest(target_vec) &&
                target_vec[4].compare("state") == 0;
        }
        else
        {
            return false;
        }
    }

    bool ApiHandler::HasMapID(const std::vector<std::string>& target_vec)
    {
        return IsMapsRequest(target_vec) && target_vec.size() > 4;
    }

    const model::Map* ApiHandler::FindMapID(const std::vector<std::string>& target_vec)
    {
        const model::Map* map_id = game_.FindMap(model::Map::Id(std::string(target_vec[4])));
        if (map_id != nullptr)
        {
            return map_id;
        }
        return nullptr;
    }  

    std::optional<std::string> ApiHandler::TryExtractToken(const std::string_view& bearer_token)
    {
        if (bearer_token.size() != 39 || bearer_token.find("Bearer") == std::string::npos)
        {
            return std::nullopt;
        }
        else
        {
            return std::string(bearer_token.substr(7, bearer_token.size()));
        }
    }
  
}  // namespace http_handler
