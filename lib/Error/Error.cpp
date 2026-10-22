#include "scc/Error/Error.h"
#include "scc/Colors/Colors.h"

using namespace scc;

void Error::print(std::ostream &O) const {
    if (Pos)
        O << Color::bold() << Pos.value() << ": ";
    else
        O << getProgramNameString() << ": " << Color::bold();

    O << getPrintLevelString() << boldText() << ": " << Msg.str() << Color::reset() << "\n";
    printCode(O);
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
        return std::string(Color::red()) + "error" + Color::reset();
    case err::DiagLevel::warning:
        return std::string(Color::magenta()) + "warning" + Color::reset();
    case err::DiagLevel::note:
        return std::string(Color::cyan()) + "note" + Color::reset();
    }
    return "";
}
