#ifndef SCC_OPTION_ARGS_H
#define SCC_OPTION_ARGS_H
///
///
/// TODO:
/// change the args map into a list so i could implement fucntion like
/// get lastOf a template <typename ...T> class that return wich is the last option to apear in the
/// list args (ex : use write scc -m32 -m64 -> this will return -m64 with getLastOf(Opt_64,
/// Opt_m32))
///
///
///

#include <cassert>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include "scc/ADT/vector.h"

namespace scc {

class Arg {
  public:
    ~Arg() = default;
    enum ValueType { None, Str, StrList };

  private:
    const int                Type;
    scc::Vector<std::string> Value;
    Arg::ValueType           Kind;
    bool                     Claim = false;

  public:
    explicit Arg(const int Type) : Type(Type), Kind(None) {}
    explicit Arg(const int Type, ValueType VType, const std::string &Str)
        : Type(Type),
          Value{Str},
          Kind(VType) {}
    explicit Arg(const int Type, const scc::Vector<std::string> &Strs)
        : Type(Type),
          Value(Strs),
          Kind(StrList) {}

    bool isFlag() const { return Kind == None; }
    bool isSingle() const { return Kind == Str; }
    bool isMulti() const { return Kind == StrList; }

    int getType() const { return Type; }
    int getValueType() const { return Kind; }

    int count() const { return Value.size(); }
    int empty() const { return Value.empty(); }

    const std::string &getValue() {
        assert(isSingle() && "getValue Should be Use only for Arg With one Value");
        return Value[0];
    }

    const scc::Vector<std::string> &getValuesList() {
        assert(isMulti() && "getValueList Should be Use only for Arg With Multiple Value");
        return Value;
    }

    // only works for StrList Arg
    bool addValueToList(const std::string &Str) {
        if (isMulti()) {
            Value.pushBack(Str);
            return true;
        }
        return false;
    }

    bool addValuesToList(const scc::Vector<std::string> &Strs) {
        assert(isMulti() && "addValuesToList Should be Use only for Arg With Multiple Value");
        for (auto s : Strs) {
            if (addValueToList(s) == true)
                return true;
        }
        return false;
    }

    void setValueSingle(const std::string &Str) {
        assert(isSingle() && "setValueSingle Should be Use only for Arg With one Value");
        Value.front() = Str;
    }

    void claim() { Claim = true; }
    bool claimed() const { return Claim; }
};

struct ArgOccurrence {
    int         Type;
    std::string Spelling;

    int                getType() const { return Type; }
    const std::string &getSpelling() const { return Spelling; }
};

class ArgsList {
    // all the possible type for the config
    using ValMapType = std::map<int, std::unique_ptr<Arg>>;

    ValMapType                    ValMap;
    scc::Vector<ArgOccurrence>    Occurrences;
    scc::Vector<std::string_view> Files;

  public:
    ~ArgsList() = default;

    Arg *getArg(int Index) const;

    int size() const { return ValMap.size(); }

    bool hasArg(int Index) const { return getArg(Index) != nullptr; }

    const scc::Vector<ArgOccurrence> &getOccurrences() const { return Occurrences; }
    const ArgOccurrence *getFirstOccurrenceOf(std::initializer_list<int> Indices) const;
    const ArgOccurrence *getLastOccurrenceOf(std::initializer_list<int> Indices) const;
    int                  countOccurrencesOf(std::initializer_list<int> Indices) const;

    void addArgFlag(std::unique_ptr<Arg> A, const std::string &Spelling = "");
    void addFile(std::string_view File);

    ValMapType::iterator begin() { return ValMap.begin(); }
    ValMapType::iterator end() { return ValMap.end(); }

    const scc::Vector<std::string_view> &getFiles() const;
};

} // namespace scc

#endif // SCC_OPTION_ARGS_H
