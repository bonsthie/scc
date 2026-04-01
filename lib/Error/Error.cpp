#include "scc/Error/Error.h"

using namespace scc;

// TODO : handle NO_COLOR
void Error::print(std::ostream &O) const {
    if (Pos)
        O << COL_BOLD << Pos.value() << ": ";
    else
        O << getProgramNameString() << ": " COL_BOLD;

    O << getPrintLevelString() << COL_BOLD ": " << Msg.str() << COL_RESET "\n";
}

int Error::emit(std::ostream &O) const {
    int E = shouldEmit();
    if (E & err::Emit)
        print(O);
    return E & err::Stop;
}

std::string Error::getPrintLevelString() const {
    switch (Level) {
    case err::DiagLevel::error:
        return COL_RED "error" COL_RESET;
    case err::DiagLevel::warning:
        return COL_MAGENTA "warning" COL_RESET;
    }
    return "";
}
