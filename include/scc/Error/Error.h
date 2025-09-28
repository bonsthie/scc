#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Error/ErrorColors.h"
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
    err::DiagLevel     Level;
    std::ostringstream Msg;

  public:
    explicit Error(err::DiagLevel Level) : Level(Level) {}

    Error &msg(const std::string &msg) {
        Msg << msg;
        return *this;
    }

    err::DiagLevel getDiagLevel() const { return Level; }
    std::string    getMsg() const { return Msg.str(); }

    virtual err::EmitionLeveL shouldEmit() const { return err::Emit; }

    // TODO : handle NO_COLOR
    // clang-format off
    virtual void print(std::ostream &O) const {
		O << getProgramNameString() << ": " 
		<< COL_BOLD 
		<< getPrintLevelString() 
		<<  COL_BOLD ": "
		<< Msg.str() << COL_RESET "\n";
    }
    // clang-format on

    int emit(std::ostream &O) const {
        int E = shouldEmit();
        if (E & err::Emit)
            print(O);
        return E & err::Stop;
    }

    std::string getPrintLevelString() const {
        switch (Level) {
        case err::DiagLevel::error:
            return COL_RED "error" COL_RESET;
        case err::DiagLevel::warning:
            return COL_MAGENTA "warning" COL_RESET;
        }
    };

    // TODO : take from the prog name
    std::string getProgramNameString() const { return "scc"; }
};

} // namespace scc

#endif //  SCC_ERROR_ERROR_H
