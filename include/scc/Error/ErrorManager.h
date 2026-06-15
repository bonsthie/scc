#ifndef SCC_ERROR_ERRORMANAGER_H
#define SCC_ERROR_ERRORMANAGER_H

#include "scc/Error/Error.h"

#include <functional>
#include <iostream>
#include <memory>
#include <ostream>

#include "scc/ADT/vector.h"

namespace scc {

class ErrorManager {
    using ErrorPtr = std::unique_ptr<Error>;

    scc::Vector<ErrorPtr> Errs;
    std::ostream         &O;

    using FactoryFunc = std::function<std::unique_ptr<Error>(err::DiagLevel)>;
    FactoryFunc Factory;

  public:
    ErrorManager(FactoryFunc FF = defaultFactory(), std::ostream &O = std::cerr)
        : O(O),
          Factory(FF) {}

    Error &report(err::DiagLevel Level) {
        auto Ptr = Factory(Level);
        Errs.emplaceBack(std::move(Ptr));
        return *Errs.back();
    }

    Error &last() { return *Errs.back(); }

    bool emit() {
        for (auto *It = Errs.begin(); It != Errs.end();) {
            int Status = (*It)->emit(O);
            It = Errs.erase(It);
            if (Status) {
                Errs.clear();
                return true;
            }
        }
        return false;
    }

    const scc::Vector<ErrorPtr> &getErrsList() const { return Errs; }

    int size() { return Errs.size(); }

    static FactoryFunc defaultFactory() {
        return [](err::DiagLevel L) { return std::make_unique<Error>(L); };
    }
};

} // namespace scc

#endif // SCC_ERROR_ERRORMANAGER_H
