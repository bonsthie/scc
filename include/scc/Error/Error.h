#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Colors/Colors.h"
#include "scc/FileManager/MemoryBufferView.h"
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace scc {

namespace err {

enum DiagLevel {
    warning,
    error,
    note,
};

enum EmitionLeveL { Emit = 1, Stop = 1 << 1, EmitStop = Emit | Stop };

enum class Code {
    OK,
    ERROR, // EM manager error has been set
};

} // namespace err

#define ERR_MSG
#define WARN_MSG

class Error {
    err::DiagLevel               Level;
    std::ostringstream           Msg;
    std::optional<PosView>       Pos;
    std::optional<MemoryViewPos> End;

  public:
    explicit Error(err::DiagLevel Level) : Level(Level) {}

    Error(Error &&) noexcept = default;
    Error &operator=(Error &&) noexcept = default;
    virtual ~Error() = default;

    Error &msg(const std::string &Message) {
        Msg << Message;
        return *this;
    }

    Error &quote(const std::string &Message) {
        Msg << "\"" << Message << "\"";
        return *this;
    }

    std::ostringstream &msg() { return Msg; }

    Error &quoteChar(char c) {
        Msg << "'" << c << "'";
        return *this;
    }

    Error &at(PosView P) {
        Pos = P;
        End.reset();
        return *this;
    }

    Error &at(const SourceRange &Range) {
        Pos = Range.posViewBegin();
        End = Range.End;
        return *this;
    }

    err::DiagLevel getDiagLevel() const { return Level; }
    std::string    getMsg() const { return Msg.str(); }

    virtual err::EmitionLeveL shouldEmit() const {
        return Level != err::error ? err::Emit : err::EmitStop;
    }

    virtual void print(std::ostream &O) const;
    void         printCode(std::ostream &O) const;
    int          emit(std::ostream &O) const;
    std::string  getPrintLevelString() const;

    // TODO : take from the prog name
    std::string getProgramNameString() const { return "scc"; }

    // return a bold escape sequence if the text should be bold
    std::string boldText() const { return Level == err::note ? "" : Color::bold(); }
};

} // namespace scc

#endif //  SCC_ERROR_ERROR_H
