#include "scraper.hpp"
#include <iostream>

std::vector<std::string> CreateTokenArray(const std::string& htmlString) {
    std::vector<std::string> tokenArray;
    std::string buffer;

    for (const auto& character : htmlString) {
        switch (character) {
            case '<':
                if (buffer.empty())
                    break;
                tokenArray.push_back(buffer);
                buffer.clear();
                break;
            case '>':
                buffer += character;
                tokenArray.push_back(buffer);
                buffer.clear();
                continue;
            default:
                break;
        }

        buffer += character;
    }

    return tokenArray; 
}

enum class TokenType {
    OPEN = 0,
    CLOSE,
    NO_CLOSING,
    NO_TAG
};

// http://xahlee.info/js/html5_non-closing_tag.html
bool IsVoidElement(const std::string& token) {
    if (token.starts_with("<area")) return true;
    if (token.starts_with("<base")) return true;
    if (token.starts_with("<br")) return true;
    if (token.starts_with("<col")) return true;
    if (token.starts_with("<embed")) return true;
    if (token.starts_with("<hr")) return true;
    if (token.starts_with("<img")) return true;
    if (token.starts_with("<input")) return true;
    if (token.starts_with("<link")) return true;
    if (token.starts_with("<meta")) return true;
    if (token.starts_with("<param")) return true;
    if (token.starts_with("<source")) return true;
    if (token.starts_with("<track")) return true;
    if (token.starts_with("<wbr")) return true;
    return false;
}

TokenType ParseToTokenType(const std::string& token) {
    if (token.starts_with("</"))
        return TokenType::CLOSE;

    if (token.starts_with("<") && token.ends_with(">")) {
        if (IsVoidElement(token))
            return TokenType::NO_CLOSING;
        return TokenType::OPEN;
    }

    return TokenType::NO_TAG;
}

std::vector<std::string> Split(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void ReplaceAll(std::string& str, const std::string& value, const std::string& replaceValue = "") {
    size_t pos;
    while ((pos = str.find(value)) != std::string::npos)
        str = str.replace(pos, value.size(), replaceValue);
}

scraper::HTMLAttribute ParseAttributeValue(const std::string& attrib) {
    std::vector<std::string> attribParts = Split(attrib, "=");

    if (attribParts.size() < 2ull)
        return scraper::HTMLAttribute { .key = attribParts.at(0), .value = "" };

    ReplaceAll(attribParts.at(1), "\"");
    
    return scraper::HTMLAttribute { .key = attribParts.at(0), .value = attribParts.at(1) };
}

scraper::HTMLElement ParseAttributes(const std::string& token) {
    std::string tokenCpy = token;
    if (auto pos = tokenCpy.find("</"); pos != std::string::npos)
        tokenCpy = tokenCpy.replace(pos, 2, "");
    else if (pos = tokenCpy.find("<"); pos != std::string::npos)
        tokenCpy = tokenCpy.replace(pos, 1, "");

    if (auto pos = tokenCpy.find("/>"); pos != std::string::npos)
        tokenCpy = tokenCpy.replace(pos, 2, "");
    else if (pos = tokenCpy.find(">"); pos != std::string::npos)
        tokenCpy = tokenCpy.replace(pos, 1, "");


    auto attributes = Split(tokenCpy, " ");

    // BUG: if no attrib 0 we crash
    scraper::HTMLElement newElement(attributes.at(0));

    for (size_t i = 1; i < attributes.size(); i++) {
        const auto& attrib = attributes.at(i);
        if (attrib != " " && !attrib.empty())
            newElement.attributes.push_back(ParseAttributeValue(attrib));
    }

    return newElement;
}

scraper::HTMLElement scraper::ParseHTML(const std::string& htmlString) {
    scraper::HTMLElement document("document");
    scraper::HTMLElement* current = &document;

    for (const std::string& token : CreateTokenArray(htmlString)) {
        switch (ParseToTokenType(token)) {
            case TokenType::OPEN: {
                scraper::HTMLElement newElement = ParseAttributes(token);
                newElement.parent = current;
                current->children.push_back(newElement);
                current = &current->children.at(current->children.size() - 1);
                break;
            }
            case TokenType::CLOSE:
                current = current->parent;
                break;
            case TokenType::NO_CLOSING: {
                scraper::HTMLElement newElement = ParseAttributes(token);
                newElement.parent = current;
                current->children.push_back(newElement);
                break;
            }
            case TokenType::NO_TAG: {
                scraper::HTMLElement newElement("");
                newElement.inner = token;
                newElement.parent = current;
                current->children.push_back(newElement);
                break;
            }
            default:
                break;
        }
    }

    return document;
}

scraper::HTMLElement::HTMLElement(const std::string& tag) {
    this->tag = tag;
}