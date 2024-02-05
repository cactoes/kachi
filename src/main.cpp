#include <iostream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include <chrono>

#include "scraper/scraper.hpp"

#define WEBSITE_TEST

int main() {
#ifdef WEBSITE_TEST
    httplib::Client client("https://www.livechart.me");
    auto res = client.Get("/timetable");

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; i++)
        scraper::ParseHTML(res->body);
    auto stop = std::chrono::high_resolution_clock::now();
#else
    std::string testHTML =
        "<hmtl>"
            "<head>"
               "<link rel=\"dns-prefetch\" href=\"https://s.livechart.me\">"
            "</head>"
            "<body>"
                "<p>"
                    "Test"
                "</p>"
                "<input class=\"hide\" type=\"checkbox\" name=\"rewatching\" data-mark-filter-modal-target=\"checkbox\" />"
            "</body>"
        "</hmtl>";

    scraper::HTMLElement document = scraper::ParseHTML(testHTML);
#endif

    auto duration = duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << (double)(duration.count()) / 10.0 << std::endl;

    return 0;
}