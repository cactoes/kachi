#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <time.h>

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
        static const std::vector<std::string> months = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "dec" };
        return months.at(m_month);
    }

    bool operator<(const Time& time) const {
        if (m_month < time.m_month)
            return true;

        if (m_day < time.m_day)
            return true;

        if (m_hour < time.m_hour)
            return true;

        return m_minute < time.m_minute;
    }

private:
    void SetTime(time_t time) {
        struct tm* ptm = gmtime(&time);
        m_hour = (ptm->tm_hour + 1) % 24;
        m_minute = ptm->tm_min;
        m_month = ptm->tm_mon;
        m_day = ptm->tm_mday;
    }

public:
    int m_month = 0;
    int m_day = 0;
    int m_hour = 0;
    int m_minute = 0;
};

#endif // __TIME_HPP__