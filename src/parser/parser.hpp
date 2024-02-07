#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace parser {
    class HTMLElement {
    public:
        std::wstring tag;
        std::wstring inner;
        std::map<std::wstring, std::wstring> attributes;
        std::vector<HTMLElement> children;
        std::vector<std::wstring> classList;
        std::wstring id;
        std::optional<HTMLElement*> parent;

        HTMLElement(const std::wstring& tag);

        std::optional<HTMLElement*> GetElementById(std::wstring idName);
        std::vector<HTMLElement*> GetElementsByClassName(const std::wstring& className);
        std::vector<HTMLElement*> GetElementsByTagName(const std::wstring& tagName);
    };

    HTMLElement ParseHTML(const std::wstring& htmlString);
}; // parser

#endif // __PARSER_HPP__