#ifndef SCC_PARSER_PARSER_H
#define SCC_PARSER_PARSER_H

#include "scc/Error/ErrorManager.h"
#include "scc/PreProcessor/PreProcessor.h"
#include "scc/Sema/Sema.h"

namespace scc {

class Parser {
    PreProcessor &PP;
    Sema         &Action;
    [[maybe_unused]] ErrorManager &EM;

    bool IsEOF = false;
    bool HasErrorOccurred = false;

  public:
    Parser(PreProcessor &PP, Sema &Action, ErrorManager &EM) : PP(PP), Action(Action), EM(EM) {}

    PreProcessor       &getPreprocessor() { return PP; }
    const PreProcessor &getPreprocessor() const { return PP; }

    Sema       &getSema() { return Action; }
    const Sema &getSema() const { return Action; }

    bool isEOF() { return IsEOF; }
    bool hasErrorOccurred() { return HasErrorOccurred; }

    Decl *parseTopLevelDecl();
};

} // namespace scc

#endif // SCC_PARSER_PARSER_H
