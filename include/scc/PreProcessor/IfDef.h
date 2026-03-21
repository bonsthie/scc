#ifndef SCC_PREPROCESSOR_IFDEF_H
#define SCC_PREPROCESSOR_IFDEF_H

#include <stack>
#include <string_view>

class IfDef {
    std::string_view Name;
};

class IfDefMgr {
    std::stack<IfDef> IfDefs;
};

#endif // SCC_PREPROCESSOR_IFDEF_H
