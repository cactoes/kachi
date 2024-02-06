#include <iostream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include <chrono>

#include "scraper/scraper.hpp"

// #define WEBSITE_TEST

int main() {
#ifdef WEBSITE_TEST
    httplib::Client client("https://www.livechart.me");
    auto res = client.Get("/timetable");

    scraper::HTMLElement document = scraper::ParseHTML(res->body);

    auto elements = document.GetElementsByClassname("timetable-day");

#else
    std::string testHTML =
        "<hmtl>"
            "<head>"
               "<link rel=\"dns-prefetch\" href=\"https://s.livechart.me\">"
            "</head>"
            "<body>"
                "<p id=\"testId\">"
                    "Test"
                "</p>"
                "<input class=\"hide button\" type=\"checkbox\" name=\"rewatching\" data-mark-filter-modal-target=\"checkbox\" />"
            "</body>"
        "</hmtl>";

    scraper::HTMLElement document = scraper::ParseHTML(testHTML);
    auto elements = document.GetElementById("testId");

#endif
    return 0;
}