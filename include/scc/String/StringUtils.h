#ifndef SCC_STRING_STRINGUTILS_H
#define SCC_STRING_STRINGUTILS_H

#include <string>

static inline std::string trim_quote(std::string &Str) {
    if (Str.size() >= 2 && Str.front() == '"' && Str.back() == '"')
        return Str.substr(1, Str.size() - 2);
    return Str;
}

static inline std::string trim_quote_system(std::string &Str) {
    if (Str.size() >= 2 && Str.front() == '<' && Str.back() == '>')
        return Str.substr(1, Str.size() - 2);
    return Str;
}

#endif
