#pragma once

#include <boost/log/utility/setup/console.hpp>

void MyFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm); 

/*#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>
#include <iostream>
#include <boost/json.hpp>
#include <boost/log/trivial.hpp>     // для BOOST_LOG_TRIVIAL
#include <boost/log/core.hpp>        // для logging::core
#include <boost/log/expressions.hpp> // для выражения, задающего фильтр 
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time.hpp>
//#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

//#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)


namespace logger {

    using namespace std::literals;
    namespace logging = boost::log;
    namespace json = boost::json;
    namespace pt = boost::posix_time;
    namespace sinks = boost::log::sinks;
    namespace keywords = boost::log::keywords;
    namespace expr = boost::log::expressions;
    namespace attrs = boost::log::attributes;

    //constexpr char MESSAGE[] = "message";
    //constexpr char DATA[] = "data";
    //constexpr char TIMESTAMP[] = "timestamp";

    BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", json::value)
    BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", pt::ptime)

    void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
        auto ts = rec[timestamp];
        strm << "{";
        strm << "\"timestamp\":" << pt::to_iso_extended_string(*ts) << ",";
        strm << "\"data\":" << boost::json::serialize(*rec[additional_data]) << ",";
        strm << "\"message\":" << rec[expr::smessage];
        strm << "}";
    }

    void InitBoostLogFilter() {
        logging::add_common_attributes();

        logging::add_console_log(
            std::cout,
            boost::log::keywords::format = &MyFormatter
        );
    }

    class Logger {

        Logger() = default;
        Logger(const Logger&) = delete;

    public:

        static Logger& GetInstance() {
            static Logger obj;
            return obj;
        }

        // Выведите в поток все аргументы.
        template<typename... Args>
        void Log(Args... s) {
            std::lock_guard g(mutex_);
            json::object log_json;
            auto ts = pt::microsec_clock::local_time();
            log_json[TIMESTAMP] = to_iso_extended_string(ts);
            json::object data_json;
            GetData(data_json, s...);
            log_json[DATA] = data_json;
            //log_json[MESSAGE] = message;
            //log_file_.close();
        }

    private:

        void GetData(json::object& data_json) {
            return;
        }

        template<typename T, typename... Args>
        void GetData(json::object& data_json, T first, T second, Args... messages) {
            data_json[first] = second;
            GetData(data_json, messages...);
        }

        std::mutex mutex_;

    };
}*/