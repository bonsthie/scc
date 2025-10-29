#include <string>

static inline std::string trim_quote(std::string &str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"')
        return str.substr(1, str.size() - 2);
    return str;
}

static inline std::string trim_quote_system(std::string &str) {
    if (str.size() >= 2 && str.front() == '<' && str.back() == '>')
        return str.substr(1, str.size() - 2);
    return str;
}
