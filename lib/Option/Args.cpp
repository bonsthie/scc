#include "scc/Option/Args.h"
#include <memory>

using namespace scc;

Arg *ArgsList::getArg(int index) const {
    auto it = ValMap.find(index);
    if (it == ValMap.end())
        return nullptr;
    return it->second.get();
}

void ArgsList::addArgFlag(std::unique_ptr<Arg> A) {
    int index = A->getType();

    if (Arg *a = getArg(index)) {
        switch (a->getValueType()) {
        case Arg::StrList: {
            auto strs = A->getValuesList();
            a->addValuesToList(strs);
            return;
        }
        case Arg::Str: {
            auto str = A->getValue();
            a->setValueSingle(str);
            return;
        }
        }
    }
    ValMap.emplace(index, std::move(A));
}
