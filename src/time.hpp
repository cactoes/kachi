#ifndef __TIME_HPP__
#define __TIME_HPP__

#include <time.h>

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
        static const std::vector<std::wstring> months = { L"jan", L"feb", L"mar", L"apr", L"may", L"jun", L"jul", L"aug", L"sep", L"oct", L"dec" };
        return months.at(m_month);
    }

    bool operator<(const Time& time) const {
        if (m_year > time.m_year)
            return false;

        if (m_month > time.m_month)
            return false;

        if (m_day > time.m_day)
            return false;

        if (m_hour > time.m_hour)
            return false;

        return m_minute < time.m_minute;
    }

private:
    void SetTime(time_t time) {
        struct tm* ptm = gmtime(&time);

        m_year = ptm->tm_year + 1900;
        m_month = ptm->tm_mon;
        m_day = ptm->tm_mday;
        m_hour = (ptm->tm_hour + 1) % 24;
        m_minute = ptm->tm_min;
    }

public:
    int m_year = 0;
    int m_month = 0;
    int m_day = 0;
    int m_hour = 0;
    int m_minute = 0;
};

#endif // __TIME_HPP__