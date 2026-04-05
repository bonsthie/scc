#include "scc/Option/Args.h"
#include <memory>

using namespace scc;

static bool matchesIndex(int index, std::initializer_list<int> indices) {
    for (int candidate : indices) {
        if (candidate == index)
            return true;
    }
    return false;
}

Arg *ArgsList::getArg(int index) const {
    auto it = ValMap.find(index);
    if (it == ValMap.end())
        return nullptr;
    return it->second.get();
}

const ArgOccurrence *ArgsList::getFirstOccurrenceOf(std::initializer_list<int> indices) const {
    for (const auto &Occurrence : Occurrences) {
        if (matchesIndex(Occurrence.Type, indices))
            return &Occurrence;
    }
    return nullptr;
}

const ArgOccurrence *ArgsList::getLastOccurrenceOf(std::initializer_list<int> indices) const {
    for (size_t i = Occurrences.size(); i > 0; --i) {
        const auto &Occurrence = Occurrences[i - 1];
        if (matchesIndex(Occurrence.Type, indices))
            return &Occurrence;
    }
    return nullptr;
}

int ArgsList::countOccurrencesOf(std::initializer_list<int> indices) const {
    int Count = 0;

    for (const auto &Occurrence : Occurrences) {
        if (matchesIndex(Occurrence.Type, indices))
            ++Count;
    }
    return Count;
}

void ArgsList::addArgFlag(std::unique_ptr<Arg> A, const std::string &Spelling) {
    int index = A->getType();
    Occurrences.push_back({index, Spelling});

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
        case Arg::None:
            assert("this function should be use for Arg without argument");
        }
    }
    ValMap.emplace(index, std::move(A));
}

void ArgsList::addFile(std::string_view File) { Files.push_back(File); }

const scc::vector<std::string_view> &ArgsList::getFiles() const { return Files; }
