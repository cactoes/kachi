#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <iostream>
#include <codecvt>
#include <memory>
#include <sstream>
#include <htmlparser.hpp>
#include <ranges>

#ifdef _WIN32
#include "Windows.h"
#endif

#include "time.hpp"

namespace LiveChartScraper {
    std::string GetRawPageContentForDay(const Time& time) {
        httplib::Client client("https://www.livechart.me");
        auto result = client.Get("/schedule?date=" + time.ToYearMonthDayString("-"));

        if (result->status != 200)
            return "";

        return result->body;
    }

    enum class AnimeEntryType {
        BASIC = 0,
        MESSAGE
    };

    struct AnimeEntry {
        Time m_time;
        std::string m_episode;
        std::string m_title;
        std::string m_message;
        AnimeEntryType m_type;
    };

    std::vector<AnimeEntry> GetEntriesFor(const std::string& content, size_t day) {
        auto document = parser::ParseHTML(content);
        auto elements = document.GetElementsByClassName("lc-timetable-day");

        std::vector<AnimeEntry> entries;

        if (elements.size() < 1)
            return entries;

        for (auto& slot : elements.at(day)->GetElementsByClassName("lc-timetable-timeslot")) {
            if (slot->attributes.at("data-controller") == "current-time")
                continue;

            if (std::ranges::find(slot->classList, "hidden") != slot->classList.end())
                continue;

            auto slotTime = Time(slot->attributes.at("data-timestamp"));

            auto contentBlock = slot->GetElementsByClassName("lc-timetable-timeslot__content").at(0);

            auto animeBlocks = contentBlock->GetElementsByClassName("lc-timetable-anime-block");
            for (const auto& block : animeBlocks) {
                AnimeEntry entry;
                entry.m_type = AnimeEntryType::BASIC;
                entry.m_time = slotTime;
                entry.m_title = block->GetElementsByClassName("lc-tt-anime-title").at(0)->children.at(0).inner;
                
                auto releaseLabel = block->GetElementsByClassName("lc-tt-release-label").at(0);
                if (releaseLabel->children.size() == 2) {
                    entry.m_episode = releaseLabel->children.at(0).children.at(0).inner; // actual episode
                } else {
                    // entry.m_episode = releaseLabel->children.at(0).inner; // other info
                    entry.m_episode = "EP?";
                }
                
                entries.push_back(entry);
            }

            auto noteBlock = contentBlock->GetElementsByClassName("lc-timetable-timeslot__note");
            if (!noteBlock.empty()) {
                AnimeEntry entry;
                entry.m_type = AnimeEntryType::MESSAGE;
                entry.m_time = slotTime;
                entry.m_title = noteBlock.at(0)->GetElementsByClassName("lc-timetable-timeslot__note__anime-title").at(0)->children.at(0).inner;

                for (auto& message : noteBlock.at(0)->children.at(4).children) {
                    if (message.children.empty())
                        continue;

                    entry.m_message = message.children.at(0).inner;
                    entries.push_back(entry);
                }
            }

            if (animeBlocks.empty() && noteBlock.empty()) {
                AnimeEntry entry;
                entry.m_type = AnimeEntryType::MESSAGE;
                entry.m_time = slotTime;
                entry.m_title = contentBlock->children.at(0).children.at(0).inner;
                entry.m_message = contentBlock->children.at(2).children.at(0).inner;
                entries.push_back(entry);
            }
        }

        return entries;
    }

    std::string FormatEntry(const AnimeEntry& entry) {
        std::stringstream ss;
        switch (entry.m_type) {
            case AnimeEntryType::BASIC:
                ss << "[" << entry.m_time.ToStringHM() << "] [" << std::left << std::setw(6) << entry.m_episode << "] " << entry.m_title;
                return ss.str();
            case AnimeEntryType::MESSAGE:
                ss << "-- "<< "(" + entry.m_time.ToStringHM() + ") " << "\033[3m\033[1m" << entry.m_title << "\033[22m\033[23m " << entry.m_message << " --";
                return ss.str();
            default:
                return ss.str();
        }
    }
}; // namespace LiveChartScraper

void DrawCursor(const Time& currentTime) {
    std::cout << "\033[3m-- " << currentTime.WeekDayToString() << " " << currentTime.m_day << " " << currentTime.MonthToString() << " " << currentTime.ToStringHM() << " --\033[23m\n";
}

int main(int argc, char** argv) {
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD originalConsoleMode = 0;

    // enable colors
    (void)GetConsoleMode(console, &originalConsoleMode);
    (void)SetConsoleMode(console, originalConsoleMode | (DWORD)ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    (void)SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "[ https://www.livechart.me/schedule | kachi ]\n";

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
        std::cout << "Failed to get data";
#ifdef _WIN32
        (void)SetConsoleMode(console, originalConsoleMode);
#endif
        return 0;
    }

    auto entries = LiveChartScraper::GetEntriesFor(content, 0);

    bool hasPrintCurrentTime = false;

    if (isDifferent) {
        hasPrintCurrentTime = true;
        std::cout << "\033[3m-- " << currentTime.m_day << " " << currentTime.MonthToString() << " --\033[23m\n";
    }

    for (const auto& entry : entries) {
        if (!hasPrintCurrentTime && currentTime < entry.m_time) {
            hasPrintCurrentTime = true;
            DrawCursor(currentTime);
        }

        std::cout << LiveChartScraper::FormatEntry(entry) + "\n";
    }

    if (!hasPrintCurrentTime)
        DrawCursor(currentTime);

#ifdef _WIN32
    (void)SetConsoleMode(console, originalConsoleMode);
#endif

    return 0;
}