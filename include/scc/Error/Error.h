#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Error/ErrorColors.h"
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

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

    virtual err::EmitionLeveL shouldEmit() { return err::Emit; }

    // clang-format off
    virtual void print(std::ostream &O) {
		O << getProgramNameString() << ": " 
		<< COL_BOLD 
		<< getPrintLevelString() 
		<<  COL_BOLD ": "
		<< Msg.str() << COL_RESET "\n";
    }
    // clang-format on

    int emit(std::ostream &O) {
        int E = shouldEmit();
        if (E & err::Emit)
            print(O);
        return E & err::Stop;
    }

    std::string getPrintLevelString() {
        switch (Level) {
        case err::DiagLevel::error:
            return COL_RED "error" COL_RESET;
        case err::DiagLevel::warning:
            return COL_MAGENTA "warning" COL_RESET;
        }
    };

	// TODO take from the prog name
    std::string getProgramNameString() { return "scc"; }
};

} // namespace scc

#endif //  SCC_ERROR_ERROR_H
