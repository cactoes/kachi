#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <time.h>
#include <string>
#include <vector>

#define NOW Time()

class Time {
public:
    Time() {
        SetTime(time(nullptr));
    }

    Time(const std::string& epoch) {
        SetTime((time_t)std::stoll(epoch));
    }

    std::string ToStringHM() const {
        return (m_hour < 10 ? "0" : "") + std::to_string(m_hour) + ":" + (m_minute < 10 ? "0" : "") + std::to_string(m_minute);
    }

    std::string MonthToString() const {
        static const std::vector<std::string> months = { "jan", "feb", "mar", "apr", "may", "jun", "ju", "aug", "sep", "oct", "nov", "dec" };
        return months.at(m_month);
    }

    std::string WeekDayToString() const {
        static const std::vector<std::string> days = { "sun", "mon", "tue", "wen", "thu", "fri", "sat" };
        return days.at(m_wday);
    }

    std::string ToYearMonthDayString(const std::string& seperator) const {
        return std::to_string(m_year) + seperator + std::to_string(m_month + 1) + seperator + std::to_string(m_day);
    }

    bool operator<(const Time& time) const {
        if (m_year != time.m_year)
            return m_year < time.m_year;
        if (m_month != time.m_month)
            return m_month < time.m_month;
        if (m_day != time.m_day)
            return m_day < time.m_day;
        if (m_hour != time.m_hour)
            return m_hour < time.m_hour;
        return m_minute < time.m_minute;
    }

private:
    void SetTime(time_t time) {
        struct tm* ptm = localtime(&time);

        m_year = ptm->tm_year + 1900;
        m_month = ptm->tm_mon;
        m_day = ptm->tm_mday;
        m_wday = ptm->tm_wday;
        m_hour = ptm->tm_hour;
        m_minute = ptm->tm_min;
    }

public:
    int m_year = 0;
    int m_month = 0;
    int m_day = 0;
    int m_wday = 0;
    int m_hour = 0;
    int m_minute = 0;
};

#endif // __TIME_HPP__