#ifndef _CYBERPOLAR_LOG_H_
#define _CYBERPOLAR_LOG_H_

#include <string>
#include <memory>
#include <cstdint>
#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <tuple>


namespace cyberpolar
{
    // 日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOW = 0,
            DEBUG = 1,
            INFO  = 2,
            WARN  = 3,
            ERROR = 4,
            FATAL = 5
        };

        static const char* ToString(LogLevel::Level level); 
    };

    // 日志事件
    class LogEvent
    {
    public:
        using ptr = std::shared_ptr<LogEvent>;

        const char* getFile() const {return m_file;}
        int32_t getLine() const {return m_line;}
        uint32_t getElapse() const {return m_elapse;}
        uint32_t getThreadId() const {return m_threadId;}
        uint32_t getFiberId() const {return m_fiberId;}
        uint64_t getTime() const {return m_time;}
        const std::string getContent() const {return m_content;}
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
        LogFormatter(const std::string& pattern);


        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:
        class FormatItem
        {
        public:
            FormatItem(const std::string& fmt = "") {};
            using ptr = std::shared_ptr<FormatItem>;
            virtual ~FormatItem();

            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        void init();
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
    };

    // 日志输出地
    class LogAppender
    {
    public:
        using ptr = std::shared_ptr<LogAppender>;
        virtual ~LogAppender();

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr value);
        LogFormatter::ptr getFormatter() const;
    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };

    // 日志器
    class Logger : public std::enable_shared_from_this<Logger>
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

        const std::string& getName() const;
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
    public:
        using ptr = std::shared_ptr<StdoutLogAppender>;

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    // 输出到文件
    class FileLogAppender : public LogAppender
    {
    public:
        using ptr = std::shared_ptr<FileLogAppender>;
        FileLogAppender(const std::string& filename);
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };
}

#endif