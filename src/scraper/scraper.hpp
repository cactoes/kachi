#ifndef __SCRAPER_HPP__
#define __SCRAPER_HPP__

#include <string>
#include <vector>

namespace scraper {
    struct HTMLAttribute {
        std::string key;
        std::string value;
    };

    class HTMLElement {
    public:
        std::string tag;
        std::string inner;
        std::vector<HTMLAttribute> attributes;
        std::vector<HTMLElement> children;

        HTMLElement(const std::string& tag);
        // HTMLElement GetElementById(std::string id);

        // DO NOT USE
        HTMLElement* parent;
    };

    HTMLElement ParseHTML(const std::string& htmlString);
}; // scraper

#endif // __SCRAPER_HPP__