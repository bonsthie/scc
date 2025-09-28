#ifndef SCC_ERROR_ERRORMANAGER_H
#define SCC_ERROR_ERRORMANAGER_H

#include "scc/Error/Error.h"

#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

namespace scc {

class ErrorManager {
    std::vector<Error> Errs;
    std::ostream      &O;

    using FactoryFunc = std::function<std::unique_ptr<Error>(err::DiagLevel)>;
    FactoryFunc Factory;

  public:
    ErrorManager(FactoryFunc FF = defaultFactory(), std::ostream &O = std::cerr)
        : O(O),
          Factory(FF) {}

    Error &report(err::DiagLevel Level) {
        auto  ptr = Factory(Level);
        auto &ref = *ptr;
        Errs.push_back(std::move(ref));
        return Errs.back();
    }

    int emit() {
        for (auto e : Errs) {
            if (e.emit(O))
                return 1;
        }
        return 0;
    }

    static FactoryFunc defaultFactory() {
        return [](err::DiagLevel L) { return std::make_unique<Error>(L); };
    }
};

} // namespace scc

#endif // SCC_ERROR_ERRORMANAGER_H
