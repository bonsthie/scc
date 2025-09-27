#include "scc/Option/Args.h"
#include <memory>

using namespace scc;
using namespace scc::opt;

Arg *ArgsList::getArg(int index) {
    auto it = ValMap.find(index);
    if (it == ValMap.end())
        return nullptr;
    return it->second.get();
}

void ArgsList::addArgFlag(std::unique_ptr<Arg> A) {
    int index = A->getType();

    if (Arg *a = getArg(index)) {
        auto strs = A->getValuesList();
        a->addValuesToList(strs);
        return;
    }
    ValMap.emplace(index, std::move(A));
}
