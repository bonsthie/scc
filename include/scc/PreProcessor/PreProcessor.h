#ifndef SCC_PREPROCESSOR_PREPROCESSOR_H
#define SCC_PREPROCESSOR_PREPROCESSOR_H

#include "scc/Token/TokenStream.h"
#include <memory>
#include <vector>

// class DefineManager;
// class IfdefManager;

namespace scc {
class PreProcessor {
    std::vector<std::unique_ptr<TokenStream>> TSList;

    // DefineManager                             DM;
    // IfdefManager                              ifdefManager;
    // LineMapper								 Mapper;

  public:
    PreProcessor() = default;

    bool next(Token &Tok);

  private:
    bool handlePP(Token &Tok);
};
} // namespace scc

#endif // SCC_PREPROCESSOR_PREPROCESSOR_H
