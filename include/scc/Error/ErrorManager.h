#ifndef SCC_ERROR_ERRORMANAGER_H
#define SCC_ERROR_ERRORMANAGER_H

#include "scc/Error/Error.h"

#include <vector>
#include <iostream>
#include <ostream>

namespace scc {
namespace err {

class ErrorManager {
  public:
    ErrorManager() : O(std::cerr) {}
    ErrorManager(std::ostream &O) : O(O) {}

    Error &report(DiagLevel Level) {
        Error Err(Level);
        Errs.push_back(std::move(Err));
        return Errs.back();
    }

    int emit() {
        for (auto e : Errs) {
            EmitionLeveL lvl = e.shouldEmit();
            if (lvl & EmitionLeveL::Emit)
                e.print(O);
            if (lvl & EmitionLeveL::Stop)
                return 1;
        }
        return 0;
    }

  private:
    std::vector<Error> Errs;
    std::ostream      &O;
};

} // namespace err
} // namespace scc

#endif // SCC_ERROR_ERRORMANAGER_H
