#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>
#include <codecvt>

#include "parser/parser.hpp"
#include "time.hpp"

#pragma warning( push )
#pragma warning( disable : _UCRT_DISABLED_WARNINGS )
std::wstring StringToWideString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}
#pragma warning( pop )

struct AnimeEntry {
    Time m_time;
    std::wstring m_episode;
    std::wstring m_title;
};

parser::HTMLElement GetPage(const Time& time) {
    httplib::Client client("https://www.livechart.me");
    httplib::Result res = client.Get("/timetable?date=" + std::to_string(time.m_year) + "-" + std::to_string(time.m_month + 1) + "-" + std::to_string(time.m_day));
    return parser::ParseHTML(StringToWideString(res->body));
}

std::vector<AnimeEntry> GetTodaysReleases(parser::HTMLElement& document) {
    auto elements = document.GetElementsByClassName(L"timetable-day");

    std::vector<AnimeEntry> entries;

    if (elements.size() < 2)
        return entries;

    for (const auto& element : elements.at(0)->GetElementsByClassName(L"timetable-timeslot")) {
        entries.push_back({
            .m_time = Time(element->attributes.at(L"data-timestamp")),
            .m_episode = element->GetElementsByClassName(L"footer").at(0)->children.at(1).inner,
            .m_title = element->GetElementsByClassName(L"title").at(0)->attributes.at(L"title")
        });
    }

    return entries;
}

int main(int, char**) {
    auto now = Time();
    auto document = GetPage(now);
    auto entries = GetTodaysReleases(document);

    (void)_setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << L"[ https://www.livechart.me/timetable ]\n";

    bool hasPrintCurrentTime = false;

    for (const auto& entry : entries) {
        if (!hasPrintCurrentTime && now < entry.m_time) {
            hasPrintCurrentTime = true;
            std::wcout << L"\033[3m-- " << now.m_day << L" " << now.MonthToString() << L" " << now.ToStringHM() << L" --\033[23m\n";
        }

        std::wcout << L"[" << entry.m_time.ToStringHM() << L"] " << std::left << std::setw(6) << entry.m_episode << L"- " << entry.m_title << L"\n";
    }

    return 0;
}