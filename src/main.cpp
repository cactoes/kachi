#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>
#include <codecvt>
#include <memory>
#include <sstream>
#include <htmlparser.hpp>

#include "time.hpp"

#pragma warning( push )
#pragma warning( disable : _UCRT_DISABLED_WARNINGS )
std::wstring StringToWideString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}
#pragma warning( pop )

namespace LiveChartScraper {
    std::wstring GetRawPageContentForDay(const Time& time) {
        httplib::Client client("https://www.livechart.me");
        auto result = client.Get("/timetable?date=" + time.ToYearMonthDayString("-"));

        if (result->status != 200)
            return L"";

        return StringToWideString(result->body);
    }

    enum class AnimeEntryType {
        BASIC = 0,
        MESSAGE
    };

    struct AnimeEntry {
        Time m_time;
        std::wstring m_episode;
        std::wstring m_title;
        std::wstring m_message;
        AnimeEntryType m_type;
    };

    std::vector<AnimeEntry> GetEntriesFor(const std::wstring& content, size_t day) {
        auto document = parser::ParseHTML(content);
        auto elements = document.GetElementsByClassName(L"timetable-day");

        std::vector<AnimeEntry> entries;

        if (elements.size() < 1)
            return entries;

        for (const auto& element : elements.at(day)->GetElementsByClassName(L"timetable-timeslot")) {
            AnimeEntry entry;
            entry.m_time = Time(element->attributes.at(L"data-timestamp"));

            bool isTypeMessage = !element->GetElementsByClassName(L"timetable-timeslot__note").empty();
            if (isTypeMessage) {
                entry.m_type = AnimeEntryType::MESSAGE;

                for (const auto& child : element->GetElementsByTagName(L"p").at(0)->children) {
                    if (child.children.empty()) {
                        entry.m_message += child.inner;
                    } else {
                        entry.m_message += L"\033[3m\033[1m" + child.children.at(0).inner + L"\033[22m\033[23m";
                    }
                }

            } else {
                if (!element->GetElementsByClassName(L"footer").empty()) {
                    entry.m_episode = element->GetElementsByClassName(L"footer").at(0)->children.at(1).inner;
                    entry.m_title = element->GetElementsByClassName(L"title").at(0)->children.at(0).inner;
                    entry.m_type = AnimeEntryType::BASIC;
                } else {
                    entry.m_message = element->GetElementsByTagName(L"p").at(0)->children.at(0).inner;
                    entry.m_type = AnimeEntryType::MESSAGE;
                }
            }

            entries.push_back(entry);
        }

        return entries;
    }

    std::wstring FormatEntry(const AnimeEntry& entry) {
        std::wstringstream ss;
        switch (entry.m_type) {
            case AnimeEntryType::BASIC:
                ss << L"[" << entry.m_time.ToStringHM() << L"] [" << std::left << std::setw(6) << entry.m_episode << L"] " << entry.m_title;
                return ss.str();
            case AnimeEntryType::MESSAGE:
                ss << L"-- " << entry.m_message << L" (" + entry.m_time.ToStringHM() + L") --";
                return ss.str();
            default:
                return ss.str();
        }
    }
}; // namespace LiveChartScraper

void DrawCursor(const Time& currentTime) {
    std::wcout << L"\033[3m-- " << currentTime.WeekDayToString() << " " << currentTime.m_day << L" " << currentTime.MonthToString() << L" " << currentTime.ToStringHM() << L" --\033[23m\n";
}

int main(int argc, char** argv) {
    (void)_setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << L"[ https://www.livechart.me/timetable | kachi ]\n";

    auto currentTime = NOW;
    bool isDifferent = false;

    if (argc > 2) {
        isDifferent = true;

        for (int i = 1; i < argc; i++) {
            if (std::string{ argv[i] } == "-d" && i + 1 < argc)
                currentTime.m_day = std::stoi(argv[i + 1]);

            if (std::string{ argv[i] } == "-m" && i + 1 < argc)
                currentTime.m_month = std::stoi(argv[i + 1]) - 1;

            if (std::string{ argv[i] } == "-y" && i + 1 < argc)
                currentTime.m_year = std::stoi(argv[i + 1]);
        }
    }

    auto content = LiveChartScraper::GetRawPageContentForDay(currentTime);
    if (content.empty()) {
        std::wcout << "Failed to get data";
        return 0;
    }

    auto entries = LiveChartScraper::GetEntriesFor(content, 0);

    bool hasPrintCurrentTime = false;

    if (isDifferent) {
        hasPrintCurrentTime = true;
        std::wcout << L"\033[3m-- " << currentTime.m_day << L" " << currentTime.MonthToString() << L" --\033[23m\n";
    }

    for (const auto& entry : entries) {
        if (!hasPrintCurrentTime && currentTime < entry.m_time) {
            hasPrintCurrentTime = true;
            DrawCursor(currentTime);
        }

        std::wcout << LiveChartScraper::FormatEntry(entry) + L"\n";
    }

    if (!hasPrintCurrentTime)
        DrawCursor(currentTime);

    return 0;
}