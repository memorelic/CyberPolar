#ifndef _CYBERPOLAR_LOG_H_
#define _CYBERPOLAR_LOG_H_

#include <string>
#include <memory>
#include <cstdint>
#include <list>


namespace cyberpolar
{
    // 日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            DEBUG = 1,
            INFO  = 2,
            WARN  = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    // 日志事件
    class LogEvent
    {
    public:
        using ptr = std::shared_ptr<LogEvent>;
    private:
        // 文件名
        const char* m_file{nullptr};
        // 行号
        int32_t m_line{0};
        // 线程ID
        uint32_t m_threadId{0};
        // 协程ID
        uint32_t m_fiberId{0};
        // 从程序启动到现在的毫秒数
        uint32_t m_elapse{0};
        // 时间戳 
        uint64_t m_time{0};
        // 内容描述
        std::string m_content{};
    };

    // 日志格式
    class LogFormatter
    {
    public:
        using ptr = std::shared_ptr<LogFormatter>;

        std::string format(LogEvent::ptr event);
    private:

    };

    // 日志输出地
    class LogAppender
    {
    public:
        using ptr = std::shared_ptr<LogAppender>;
        virtual ~LogAppender();

        void log(LogLevel::Level level, LogEvent::ptr event);
    private:
        LogLevel::Level m_level;
    };

    // 日志器
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
    
        Logger(const std::string& name = "root");

        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const;
        void setLevel(LogLevel::Level value);
    private:
        // 日志器名称
        std::string m_name;
        // 满足此日志级别才会被输出到Appender里
        LogLevel::Level m_level;
        std::list<LogAppender::ptr> m_appenders;
    };

    // 输出到控制台
    class StdoutLogAppender : public LogAppender
    {

    };

    // 输出到文件
    class FileLogAppender : public LogAppender
    {

    };
}

#endif