#include <iostream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

int main() {
    httplib::Client client("https://www.livechart.me");

    auto res = client.Get("/timetable");

    std::cout << res->status << std::endl;

    return 0;
}