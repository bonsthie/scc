#ifndef SCC_PARSER_DELIMITERTRACKER_H
#define SCC_PARSER_DELIMITERTRACKER_H

#include "scc/Parser/Parser.h"
#include <cassert>

namespace scc {

// Tracks one delimiter pair. Consumption and diagnostics stay explicit;
// destruction never advances the token stream or attempts error recovery.
class DelimiterTracker {
    Parser        &P;
    tok::TokenKind OpenKind;
    tok::TokenKind CloseKind;
    Token          OpenTok;

    static tok::TokenKind closingKind(tok::TokenKind Open) {
        switch (Open) {
        case tok::l_paren:
            return tok::r_paren;
        case tok::l_square:
            return tok::r_square;
        case tok::l_brace:
            return tok::r_brace;
        default:
            assert(false && "expected an opening delimiter");
            return tok::unknown;
        }
    }

  public:
    explicit DelimiterTracker(Parser &P, tok::TokenKind Open)
        : P(P),
          OpenKind(Open),
          CloseKind(closingKind(Open)) {}

    // Both operations return true on success, like Parser::expect().
    bool consumeOpen() {
        assert(OpenTok.is(tok::not_init) && "opening delimiter already consumed");
        if (P.CurTok.isNot(OpenKind)) {
            P.HasErrorOccurred = true;
            P.EM.expectedX(P.CurTok.getRange(), OpenKind);
            return false;
        }
        OpenTok = P.CurTok;
        P.next();
        return true;
    }

    bool consumeClose() {
        assert(OpenTok.is(OpenKind) && "consume the opening delimiter first");
        if (P.CurTok.isNot(CloseKind)) {
            P.HasErrorOccurred = true;
            P.EM.expectedX(P.CurTok.getRange(), CloseKind);
            P.EM.toClose(OpenTok);
            return false;
        }
        P.next();
        return true;
    }
};

} // namespace scc

#endif // SCC_PARSER_DELIMITERTRACKER_H
