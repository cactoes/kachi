#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>

#include "parser/parser.hpp"
#include "time.hpp"

struct AnimeEntry {
    Time m_time;
    std::string m_episode;
    std::string m_title;
};

parser::HTMLElement GetPage(const Time& time) {
    httplib::Client client("https://www.livechart.me");
    httplib::Result res = client.Get("/timetable?date=" + std::to_string(time.m_year) + "-" + std::to_string(time.m_month + 1) + "-" + std::to_string(time.m_day));
    return parser::ParseHTML(res->body);
}

std::vector<AnimeEntry> GetTodaysReleases(parser::HTMLElement& document) {
    auto elements = document.GetElementsByClassName("timetable-day");

    std::vector<AnimeEntry> entries;

    if (elements.size() < 2)
        return entries;

    for (const auto& element : elements.at(0)->GetElementsByClassName("timetable-timeslot")) {
        entries.push_back({
            .m_time = Time(element->attributes.at("data-timestamp")),
            .m_episode = element->GetElementsByClassName("footer").at(0)->children.at(1).inner,
            .m_title = element->GetElementsByClassName("title").at(0)->attributes.at("title")
        });
    }

    return entries;
}

int main(int, char**) {
    auto now = Time();
    auto document = GetPage(now);
    auto entries = GetTodaysReleases(document);

    std::cout << "[ https://www.livechart.me/timetable ]\n";

    bool hasPrintCurrentTime = false;

    for (const auto& entry : entries) {
        if (!hasPrintCurrentTime && now < entry.m_time) {
            hasPrintCurrentTime = true;
            std::cout << "\033[3m-- " << now.m_day << " " << now.MonthToString() << " " << now.ToStringHM() << " --\033[23m\n";
        }

        std::cout << "[" << entry.m_time.ToStringHM() << "] " << std::left << std::setw(6) << entry.m_episode << "- " << entry.m_title << "\n";
    }

    return 0;
}