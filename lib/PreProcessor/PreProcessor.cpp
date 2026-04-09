#include "scc/PreProcessor/PreProcessor.h"
#include "scc/String/StringInterner.h"
#include "scc/String/StringUtils.h"
#include "scc/Token/Token.h"
#include <cassert>
#include <string>
#include <string_view>

using namespace scc;

const LangOpt &PreProcessor::defaultLangOpt() {
    static const LangOpt Opts{};
    return Opts;
}

PreProcessor::PreProcessor(File &F, ErrorManager &EM, FileManager &FM, StringInterner &SI,
                           const LangOpt &Opts)
    : EM(EM),
      FM(FM),
      Opts(Opts),
      SI(SI) {
    addNewTokenStream(F);
}

bool PreProcessor::nextRaw(Token &Tok) {
    if (CurrentTokStream == nullptr) {
        CurrentTokStream = getNextTokenStream();
        if (CurrentTokStream == nullptr) {
            Tok.setTokenKind(tok::eof);
            return true;
        }
    }

    bool err = CurrentTokStream->next(Tok);

    if (err) {
        if (Tok.is(tok::eof)) {
            popTokenStream();
            return next(Tok);
        } else
            return true;
    }

    return false;
}

bool PreProcessor::next(Token &Tok) {
    if (nextRaw(Tok))
        return true;

    if (Tok.is(tok::pp_hash)) {
        if (handlePP(Tok))
            return true;
        Tok.flush();
        return next(Tok);
    }
    return false;
}

bool PreProcessor::handlePP(Token &Tok) {
    FileLexer *FL = static_cast<FileLexer *>(CurrentTokStream);
    assert(FL && "handlePP can't be use with other things than FileLexer");

    bool err = nextRaw(Tok);

    switch (Tok.getTokenKind()) {
    case tok::pp_include:
        return handleInclude(Tok, *FL);

    case tok::unknown:
    case tok::identifier: {
        std::string value(Tok.getValue());
        EM.report(err::error).msg("'" + value + "' Invalid PreProcessing directive");
        return true;
    }

    default:
        EM.report(err::error)
            .msg("'" + stringify_token_kind(Tok.getTokenKind()) +
                 "' This preprocessor is not handle yet");
        return true;
    }

    return err;
}

bool PreProcessor::handleInclude(Token &Tok, FileLexer &FL) {

    if (FL.lexInclude(Tok))
        return true;

    switch (Tok.getTokenKind()) {

    case tok::string_literal: {
        std::string path(Tok.getValue());
        path = trim_quote(path);
        File *F = FM.getFile(path, FL.getFID());

        if (F == nullptr) {
            EM.last().at(Tok.posViewBegin());
            return true;
        }
        addNewTokenStream(*F);
        return false;
    }

    case tok::system_string: {
        std::string path(Tok.getValue());
        path = trim_quote_system(path);
        File *F = FM.getSystemFile(path);

        if (F == nullptr) {
            EM.last().at(Tok.posViewBegin());
            return true;
        }
        addNewTokenStream(*F);
        return false;
    }

    default: {
        EM.report(err::error).at(Tok.posViewBegin()).msg("expected \"FILENAME\" or <FILENAME>");
        return true;
    }
    }
}
