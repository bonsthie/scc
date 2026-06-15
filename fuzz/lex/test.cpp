#include "scc/FileManager/FileID.h"
#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Lex/FileLexer.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"

#include <cstddef>
#include <cstdint>

namespace {

volatile std::size_t GTokenSink = 0;

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *Data, std::size_t Size) {
    scc::FrontendErrorManager ErrorManager;
    scc::BumpAllocator        Arena;
    scc::StringInterner       StringInterner(Arena);
    scc::FileID               FileId("<fuzz-input>", 1);

    scc::MemoryBufferView Buffer(reinterpret_cast<const char *>(Data), Size);
    scc::FileLexer        Lexer(std::move(Buffer), StringInterner, FileId, ErrorManager);

    scc::Token Token;
    bool       SawTerminal = false;

    while (!SawTerminal) {
        const bool Stop = Lexer.next(Token);
        GTokenSink += static_cast<std::size_t>(Token.getTokenKind());
        GTokenSink += Token.getValue().size();
        GTokenSink += Token.getDirtyValue().size();
        GTokenSink += Token.getPosBegin().Buff;
        GTokenSink += Token.getPosEnd().Buff;

        SawTerminal = Stop || Token.getTokenKind() == scc::tok::eof;
    }

    GTokenSink += static_cast<std::size_t>(ErrorManager.size());
    return 0;
}
