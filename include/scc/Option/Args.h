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
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace scc {

class Arg {
  public:
    ~Arg() = default;
    enum valueType { None, Str, StrList };

  private:
    const int                Type;
    std::vector<std::string> Value;
    Arg::valueType           ValueType;
    bool                     Claim = false;

  public:
    explicit Arg(const int type) : Type(type), ValueType(None) {}
    explicit Arg(const int type, valueType VType, const std::string &str)
        : Type(type),
          ValueType(VType),
          Value{str} {}
    explicit Arg(const int type, const std::vector<std::string> &strs)
        : Type(type),
          ValueType(StrList),
          Value(strs) {}

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

    const std::vector<std::string> &getValuesList() {
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

    bool addValuesToList(const std::vector<std::string> &strs) {
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

class ArgsList {
    // all the possible type for the config
    std::map<int, std::unique_ptr<Arg>> ValMap;

  public:
    ~ArgsList() = default;

    Arg *getArg(int index) const;

    int size() const { return ValMap.size(); }

    bool hasArg(int index) { return getArg(index) != nullptr; }

    void addArgFlag(std::unique_ptr<Arg> A);
};

} // namespace scc

#endif // SCC_OPTION_ARGS_H
