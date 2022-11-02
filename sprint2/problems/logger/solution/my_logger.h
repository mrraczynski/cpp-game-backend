#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

constexpr const char* logs_path_ = "/var/log/" ;

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const
    {       
        auto time = GetTime();
        const std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(time) };

        std::stringstream month;
        month << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.month());
        std::stringstream day;
        day << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd.day());

        std::string name("sample_log_" + 
            std::to_string(static_cast<int>(ymd.year())) + "_" + 
            month.str() + "_" +
            day.str() + ".log");
        return name;
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    void LogMessage() {
        log_file_ << std::endl;
        return;
    }
    template<typename T, typename... Args>
    void LogMessage(T first, Args... messages) {
        log_file_ << first << " "sv;
        LogMessage(messages...);
    }

    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<typename... Args>
    void Log(Args... s) {
        std::lock_guard g(mutex_);
        std::string str = logs_path_ + GetFileTimeStamp();
        log_file_.open(logs_path_ + GetFileTimeStamp(), std::ios::app);
        /*if (log_file_.good())
        {
            //, std::ios::app | std::ios::out 
            log_file_ = std::ofstream { logs_path_ + GetFileTimeStamp() };
        }
        else
        {
            log_file_ = std::ofstream{ logs_path_ + GetFileTimeStamp() };
        }*/
        log_file_ << GetTimeStamp() << ": "sv;
        LogMessage(s...);
        log_file_.close();
    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts)
    {
        std::lock_guard g(mutex_);
        manual_ts_ = ts;
    }

private:
    std::mutex mutex_;

    // для демонстрации пока оставим файл в текущей директории
    std::ofstream log_file_;

    std::optional<std::chrono::system_clock::time_point> manual_ts_;
};
