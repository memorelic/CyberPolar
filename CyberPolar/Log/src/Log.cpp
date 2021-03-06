#include "Log.h"

#include <map>
#include <functional>

class LogFormatter;

namespace cyberpolar
{

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getElapse();   
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << logger->getName();  
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << std::endl;  
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string& str)
        : m_string{str}
        {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;  
        }
    private:
        std::string m_string;
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadId();  
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberId();  
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%s")
        : m_format{format}
        {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getTime();  
        }
    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();  
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();  
        }
    };

    const char* LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
            break;
        case LogLevel::INFO:
            return "INFO";
            break;
        case LogLevel::WARN:
            return "WARN";
            break;
        case LogLevel::ERROR:
            return "ERROR";
            break;
        case LogLevel::FATAL:
            return "FATAL";
            break;
        default:
            break;
        }

        return "UNKNOW";
    }

    Logger::Logger(const std::string& name)
    : m_name{name}
    {
        
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
        {
            if (*it == appender)
            {
                m_appenders.erase(it);
                return;
            }
        }
    }

    LogLevel::Level Logger::getLevel() const
    {
        return m_level;
    }
    void Logger::setLevel(LogLevel::Level value)
    {
        m_level = value;
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            auto self = shared_from_this();

            for (auto& i : m_appenders)
            {
                i->log(self, level, event);
            }
        }
    }

    const std::string& Logger::getName() const
    {
        return m_name;
    }

    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    LogFormatter::LogFormatter(const std::string& pattern)
    : m_pattern{pattern}
    {
        
    }

    void LogFormatter::init()
    {
        // str, format, type
        std::vector<std::tuple<std::string, std::string, int>> vec;

        // current string
        std::string nstr;
        const char pattern = '%';

        for (size_t i = 0; i < m_pattern.size(); i++)
        {   
            if (m_pattern[i] == pattern)
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i+1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == pattern)
                {
                    nstr.append(1, pattern);
                    continue;
                }
            }
            
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;

            while (n < m_pattern.size())
            {
                if (isspace(m_pattern[n]))
                {
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; // ????????????
                        fmt_begin = n;
                        // ++n;
                        n++;
                        continue;
                    }
                }
                if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin);
                        fmt_status = 2;
                        n++;
                        break;
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                }
                str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << 
                m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<pattern_error>", fmt, 0));
            }
            else if (fmt_status = 2)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, "", 0));
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define Create(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}
            Create(m, MessageFormatItem),
            Create(p, LevelFormatItem),
            Create(r, ElapseFormatItem),
            Create(c, NameFormatItem),
            Create(t, ThreadIdFormatItem),
            Create(n, NewLineFormatItem),
            Create(d, DateTimeFormatItem),
            Create(f, FilenameFormatItem),
#undef Create
        };

        for (auto& i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                }
                else
                {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }

            std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
        }

        /*
            %m - ?????????
            %p - ??????level
            %r - ??????????????????
            %c - ????????????
            %t - ??????ID
            %n - ????????????
            %d - ?????????
            %f - ?????????
            %l - ??????
            %% - (??????)
        */ 
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;

        // Format all items
        
        for (auto& i : m_items)
        {
            i->format(ss, logger, level, event);
        }

        return ss.str();
    }

    void LogAppender::setFormatter(LogFormatter::ptr value)
    {
        m_formatter = value;
    }

    LogFormatter::ptr LogAppender::getFormatter() const
    {
        return m_formatter;
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) 
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename{filename}
    {

    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }

        m_filestream.open(m_filename);
        
        // ?????????????????????????????????????????????
        return !!m_filestream;
    }
}
