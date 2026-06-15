#include "scc/Option/Args.h"
#include <memory>

using namespace scc;

static bool matches_index(int Index, std::initializer_list<int> Indices) {
    for (int Candidate : Indices) {
        if (Candidate == Index)
            return true;
    }
    return false;
}

Arg *ArgsList::getArg(int Index) const {
    auto It = ValMap.find(Index);
    if (It == ValMap.end())
        return nullptr;
    return It->second.get();
}

const ArgOccurrence *ArgsList::getFirstOccurrenceOf(std::initializer_list<int> Indices) const {
    for (const auto &Occurrence : Occurrences) {
        if (matches_index(Occurrence.Type, Indices))
            return &Occurrence;
    }
    return nullptr;
}

const ArgOccurrence *ArgsList::getLastOccurrenceOf(std::initializer_list<int> Indices) const {
    for (size_t i = Occurrences.size(); i > 0; --i) {
        const auto &Occurrence = Occurrences[i - 1];
        if (matches_index(Occurrence.Type, Indices))
            return &Occurrence;
    }
    return nullptr;
}

int ArgsList::countOccurrencesOf(std::initializer_list<int> Indices) const {
    int Count = 0;

    for (const auto &Occurrence : Occurrences) {
        if (matches_index(Occurrence.Type, Indices))
            ++Count;
    }
    return Count;
}

void ArgsList::addArgFlag(std::unique_ptr<Arg> A, const std::string &Spelling) {
    int Index = A->getType();
    Occurrences.pushBack({Index, Spelling});

    if (Arg *a = getArg(Index)) {
        switch (a->getValueType()) {
        case Arg::StrList: {
            auto Strs = A->getValuesList();
            a->addValuesToList(Strs);
            return;
        }
        case Arg::Str: {
            auto Str = A->getValue();
            a->setValueSingle(Str);
            return;
        }
        case Arg::None:
            static_assert("this function should be use for Arg without argument");
        }
    }
    ValMap.emplace(Index, std::move(A));
}

void ArgsList::addFile(std::string_view File) { Files.pushBack(File); }

const scc::Vector<std::string_view> &ArgsList::getFiles() const { return Files; }
