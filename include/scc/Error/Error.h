#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Error/ErrorColors.h"
#include "scc/Token/Token.h"
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

enum EmitionLeveL {
    Emit = 1,
    Stop = 1 << 1,
};

} // namespace err

#define ERR_MSG
#define WARN_MSG

class Error {
    err::DiagLevel                Level;
    std::ostringstream            Msg;
    std::optional<Token::PosView> Pos;

  public:
    explicit Error(err::DiagLevel Level) : Level(Level) {}

    Error(Error &&) noexcept = default;
    Error &operator=(Error &&) noexcept = default;
    virtual ~Error() = default;

    Error &msg(const std::string &msg) {
        Msg << msg;
        return *this;
    }

	Error &Char(char c) {
		Msg << "'" << c << "'";
		return *this;
	}

    Error &at(Token::PosView P) {
        Pos = P;
        return *this;
    }

    err::DiagLevel getDiagLevel() const { return Level; }
    std::string    getMsg() const { return Msg.str(); }

    virtual err::EmitionLeveL shouldEmit() const { return err::Emit; }

    virtual void print(std::ostream &O) const;
    int          emit(std::ostream &O) const;
    std::string  getPrintLevelString() const;

    // TODO : take from the prog name
    std::string getProgramNameString() const { return "scc"; }
};

} // namespace scc

#endif //  SCC_ERROR_ERROR_H
