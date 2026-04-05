#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Error/ErrorColors.h"
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
};

enum EmitionLeveL { Emit = 1, Stop = 1 << 1, EmitStop = Emit | Stop };

} // namespace err

#define ERR_MSG
#define WARN_MSG

class Error {
    err::DiagLevel         Level;
    std::ostringstream     Msg;
    std::optional<PosView> Pos;

  public:
    explicit Error(err::DiagLevel Level) : Level(Level) {}

    Error(Error &&) noexcept = default;
    Error &operator=(Error &&) noexcept = default;
    virtual ~Error() = default;

    Error &msg(const std::string &msg) {
        Msg << msg;
        return *this;
    }

    std::ostringstream &msg() { return Msg; }

    Error &Char(char c) {
        Msg << "'" << c << "'";
        return *this;
    }

    Error &at(PosView P) {
        Pos = P;
        return *this;
    }

    err::DiagLevel getDiagLevel() const { return Level; }
    std::string    getMsg() const { return Msg.str(); }

    virtual err::EmitionLeveL shouldEmit() const {
        return Level == err::warning ? err::Emit : err::EmitStop;
    }

    virtual void print(std::ostream &O) const;
    int          emit(std::ostream &O) const;
    std::string  getPrintLevelString() const;

    // TODO : take from the prog name
    std::string getProgramNameString() const { return "scc"; }
};

} // namespace scc

#endif //  SCC_ERROR_ERROR_H
