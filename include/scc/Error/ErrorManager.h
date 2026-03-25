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

    scc::vector<ErrorPtr> Errs;
    std::ostream         &O;

    using FactoryFunc = std::function<std::unique_ptr<Error>(err::DiagLevel)>;
    FactoryFunc Factory;

  public:
    ErrorManager(FactoryFunc FF = defaultFactory(), std::ostream &O = std::cerr)
        : O(O),
          Factory(FF) {}

    Error &report(err::DiagLevel Level) {
        auto Ptr = Factory(Level);
        Errs.emplace_back(std::move(Ptr));
        return *Errs.back();
    }

    Error &last() { return *Errs.back(); }

    bool emit() {
        for (auto it = Errs.begin(); it != Errs.end();) {
            int status = (*it)->emit(O);
            it = Errs.erase(it);
            if (status) {
                return true;
            }
        }
        return false;
    }

    const scc::vector<ErrorPtr> &getErrsList() const { return Errs; }

    int size() { return Errs.size(); }

    static FactoryFunc defaultFactory() {
        return [](err::DiagLevel L) { return std::make_unique<Error>(L); };
    }
};

} // namespace scc

#endif // SCC_ERROR_ERRORMANAGER_H
