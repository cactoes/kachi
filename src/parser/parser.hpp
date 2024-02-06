#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <vector>
#include <map>

namespace parser {
    class HTMLElement {
    public:
        std::string tag;
        std::string inner;
        std::map<std::string, std::string> attributes;
        std::vector<HTMLElement> children;
        std::vector<std::string> classList;
        std::string id;
        HTMLElement* parent;

        HTMLElement(const std::string& tag);

        HTMLElement* GetElementById(std::string idName);
        std::vector<HTMLElement*> GetElementsByClassname(const std::string& className);
    };

    HTMLElement ParseHTML(const std::string& htmlString);
}; // parser

#endif // __PARSER_HPP__