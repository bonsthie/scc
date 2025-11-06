#include "scc/PreProcessor/PreProcessor.h"
#include "scc/String/StringUtils.h"
#include "scc/Token/Token.h"
#include <cassert>

using namespace scc;

PreProcessor::PreProcessor(File &F, ErrorManager &EM, FileManager &FM) : EM(EM), FM(FM) {
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

    if (Tok.is(tok::pp_include)) {
        return handleInclude(Tok, *FL);
    } else {
        if (Tok.is(tok::unknown)) {
            // i now that is dosen't handle the pp range and can be a while token but that just a
            // place holder
            EM.report(err::error).msg("unknown preprocessor");
        } else
            EM.report(err::error).msg("this preprocessor is not handle yet");
        return true;
    }

    return err;
}

bool PreProcessor::handleInclude(Token &Tok, FileLexer &FL) {

    if (FL.lexInclude(Tok))
        return true;

    switch (Tok.getTokenKind()) {

    case tok::string_literal: {
        std::string path = Tok.getValue();
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
        std::string path = Tok.getValue();
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
