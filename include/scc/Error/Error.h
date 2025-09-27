#ifndef SCC_ERROR_ERROR_H
#define SCC_ERROR_ERROR_H

#include "scc/Error/ErrorColors.h"
#include <ostream>
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

#define ERR_MSG
#define WARN_MSG

class Error {
    DiagLevel                Level;
    std::vector<std::string> msgs;

  public:
    explicit Error(DiagLevel Level) : Level(Level) {}

    Error &msg(std::string msg) {
        msgs.push_back(std::move(msg));
        return *this;
    }

    virtual EmitionLeveL shouldEmit() { return Emit; }
    virtual void         print(std::ostream &O) {
        // Prefix (e.g. "scc error: " or "scc warning: ")
        if (Level == DiagLevel::error) {
            O << "scc " << "Error ";
        } else {
            O << "scc " << " Warning ";
        }

        // Join all message pieces into one string
        bool first = true;
        for (auto &msg : msgs) {
            if (!first)
                O << " ";
            O << msg;
            first = false;
        }
        O << std::endl;
    }

    int emit(std::ostream &O) {
        int E = shouldEmit();
        if (E & Emit)
            print(O);
        return E & Stop;
    }
};

} // namespace err
} // namespace scc

#endif //  SCC_ERROR_ERROR_H
