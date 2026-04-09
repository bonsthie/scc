#include "scc/Colors/Colors.h"
#include "scc/Error/Error.h"

using namespace scc;

void Error::print(std::ostream &O) const {
    if (Pos)
        O << Color::bold() << Pos.value() << ": ";
    else
        O << getProgramNameString() << ": " << Color::bold();

    O << getPrintLevelString() << Color::bold() << ": " << Msg.str() << Color::reset() << "\n";
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
    }
    return "";
}
