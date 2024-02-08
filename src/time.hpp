#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <time.h>

#define NOW Time()

class Time {
public:
    Time() {
        SetTime(time(nullptr));
    }

    Time(const std::wstring& epoch) {
        SetTime((time_t)std::stoll(epoch));
    }

    std::wstring ToStringHM() const {
        return (m_hour < 10 ? L"0" : L"") + std::to_wstring(m_hour) + L":" + (m_minute < 10 ? L"0" : L"") + std::to_wstring(m_minute);
    }

    std::wstring MonthToString() const {
        static const std::vector<std::wstring> months = { L"jan", L"feb", L"mar", L"apr", L"may", L"jun", L"jul", L"aug", L"sep", L"oct", L"nov", L"dec" };
        return months.at(m_month);
    }

    std::wstring WeekDayToString() const {
        static const std::vector<std::wstring> days = { L"sun", L"mon", L"tue", L"wen", L"thu", L"fri", L"sat" };
        return days.at(m_wday);
    }

    std::wstring ToYearMonthDayString(const std::wstring& seperator) const {
        return std::to_wstring(m_year) + seperator + std::to_wstring(m_month + 1) + seperator + std::to_wstring(m_day);
    }

    std::string ToYearMonthDayString(const std::string& seperator) const {
        return std::to_string(m_year) + seperator + std::to_string(m_month + 1) + seperator + std::to_string(m_day);
    }

    bool operator<(const Time& time) const {
        if (m_year <= time.m_year)
            return true;

        if (m_month <= time.m_month)
            return true;

        if (m_day <= time.m_day)
            return true;

        if (m_hour <= time.m_hour)
            return true;

        if (m_minute <= time.m_minute)
            return true;

        return false;
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