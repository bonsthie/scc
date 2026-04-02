#include "scc/Error/ErrorManager.h"
#include "scc/FileManager/FileID.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/FileLexer.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"

#include <cstddef>
#include <cstdint>

namespace {

volatile std::size_t gTokenSink = 0;

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
    scc::ErrorManager   errorManager;
    scc::StringInterner stringInterner;
    scc::FileID         fileId("<fuzz-input>", 1);

    scc::MemoryBufferView buffer(reinterpret_cast<const char *>(data), size);
    scc::FileLexer        lexer(std::move(buffer), stringInterner, fileId, errorManager);

    scc::Token token;
    bool       sawTerminal = false;

    while (!sawTerminal) {
        const bool stop = lexer.next(token);
        gTokenSink += static_cast<std::size_t>(token.getTokenKind());
        gTokenSink += token.getValue().size();
        gTokenSink += token.getDirtyValue().size();
        gTokenSink += token.getPosBegin().Buff;
        gTokenSink += token.getPosEnd().Buff;

        sawTerminal = stop || token.getTokenKind() == scc::tok::eof;
    }

    gTokenSink += static_cast<std::size_t>(errorManager.size());
    return 0;
}
