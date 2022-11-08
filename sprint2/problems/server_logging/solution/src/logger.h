#pragma once

#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/date_time.hpp>

namespace logger {

    BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
    BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value)

    void InitBoostLog();
    
    void MyFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);


    struct LoggerData {
    public:

        template<typename... Args>
        static boost::json::value GetDataJson(Args... s) {
            boost::json::object data_json;
            GetData(data_json, s...);
            return boost::json::value(data_json);
        }

    private:

        static void GetData(boost::json::object& data_json) {
            return;
        }

        template<typename F, typename S, typename... Args>
        static void GetData(boost::json::object& data_json, F first, S second, Args... messages) {
            if (static_cast<std::string>(second).size() == 0)
            {
                data_json[first] = nullptr;
            }
            else
            {
                data_json[first] = second;
            }
            GetData(data_json, messages...);
        }

    };
}