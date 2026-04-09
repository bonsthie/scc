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
    enum valueType { None, Str, StrList };

  private:
    const int                Type;
    scc::vector<std::string> Value;
    Arg::valueType           ValueType;
    bool                     Claim = false;

  public:
    explicit Arg(const int type) : Type(type), ValueType(None) {}
    explicit Arg(const int type, valueType VType, const std::string &str)
        : Type(type),
          Value{str},
          ValueType(VType) {}
    explicit Arg(const int type, const scc::vector<std::string> &strs)
        : Type(type),
          Value(strs),
          ValueType(StrList) {}

    bool isFlag() const { return ValueType == None; }
    bool isSingle() const { return ValueType == Str; }
    bool isMulti() const { return ValueType == StrList; }

    int getType() const { return Type; }
    int getValueType() const { return ValueType; }

    int count() const { return Value.size(); }
    int empty() const { return Value.empty(); }

    const std::string &getValue() {
        assert(isSingle() && "getValue Should be Use only for Arg With one Value");
        return Value[0];
    }

    const scc::vector<std::string> &getValuesList() {
        assert(isMulti() && "getValueList Should be Use only for Arg With Multiple Value");
        return Value;
    }

    // only works for StrList Arg
    bool addValueToList(const std::string &str) {
        if (isMulti()) {
            Value.push_back(str);
            return true;
        }
        return false;
    }

    bool addValuesToList(const scc::vector<std::string> &strs) {
        assert(isMulti() && "addValuesToList Should be Use only for Arg With Multiple Value");
        for (auto s : strs) {
            if (addValueToList(s) == true)
                return true;
        }
        return false;
    }

    void setValueSingle(const std::string &str) {
        assert(isSingle() && "setValueSingle Should be Use only for Arg With one Value");
        Value.front() = str;
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
    scc::vector<ArgOccurrence>    Occurrences;
    scc::vector<std::string_view> Files;

  public:
    ~ArgsList() = default;

    Arg *getArg(int index) const;

    int size() const { return ValMap.size(); }

    bool hasArg(int index) const { return getArg(index) != nullptr; }

    const scc::vector<ArgOccurrence> &getOccurrences() const { return Occurrences; }
    const ArgOccurrence *getFirstOccurrenceOf(std::initializer_list<int> indices) const;
    const ArgOccurrence *getLastOccurrenceOf(std::initializer_list<int> indices) const;
    int                  countOccurrencesOf(std::initializer_list<int> indices) const;

    void addArgFlag(std::unique_ptr<Arg> A, const std::string &Spelling = "");
    void addFile(std::string_view File);

    ValMapType::iterator begin() { return ValMap.begin(); }
    ValMapType::iterator end() { return ValMap.end(); }

    const scc::vector<std::string_view> &getFiles() const;
};

} // namespace scc

#endif // SCC_OPTION_ARGS_H
