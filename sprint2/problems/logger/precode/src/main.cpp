#include "my_logger.h"

/*#define LOG(X) Logger::GetInstance().Log([&](std::ostream& o) { o << X; })

class Logger {
    static inline std::_Timeobj<char, const tm*> GetTimeStamp() {
        const auto now = std::chrono::system_clock::now();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

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

    template<typename... Args>
    void Log(Args... s) {
        std::lock_guard g(mutex_);
        log_file_ << GetTimeStamp() << ": "sv;
        LogMessage(s...);
    }

private:
    template<typename Arg>
    Arg GetArg(Arg a)
    {
        return a;
    }

    std::mutex mutex_;

    // для демонстрации пока оставим файл в текущей директории
    std::ofstream log_file_{ "logs/sample.log"s };
};*/

void LogIndexInThread(int f, int i) {
    //LOG("Thread " << f << " index " << i);
    Logger::GetInstance().Log(f, i);
}

int main() {
    std::thread thread1([]() {
        for (int i = 0; i < 1000; ++i) {
            LogIndexInThread(1, i);
        }
        });
    std::thread thread2([]() {
        for (int i = 0; i < 1000; ++i) {
            LogIndexInThread(2, i);
        }
        });

    thread1.join();
    thread2.join();
}
