#ifndef SCC_OPTION_ARGS_H
#define SCC_OPTION_ARGS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace scc {
namespace opt {

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
    explicit Arg(const int type, valueType VType, const std::string &str) : Type(type), ValueType(VType), Value{str} {}
    explicit Arg(const int type, const std::vector<std::string> &strs)
        : Type(type),
          ValueType(StrList),
          Value(strs) {}

    bool isFlag() const { return ValueType == None; }
    bool isSingle() const { return ValueType == Str; }
    bool isMulti() const { return ValueType == StrList; }

    int getType() const { return Type; }

    const std::string *getValue() {
        if (isSingle())
            return &Value[0];
        return nullptr;
    }

    const std::vector<std::string> *getValuesList() {
        if (isMulti())
            return &Value;
        return nullptr;
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
        for (auto s : strs) {
            if (addValueToList(s) == true)
                return true;
        }
        return false;
    }

    void claim() { Claim = true; }
    bool claimed() const { return Claim; }
};

class ArgsList {
    // all the possible type for the config
    std::map<int, std::unique_ptr<Arg>> ValMap;

  public:
    ~ArgsList() = default;

    bool hasArg(int index) { return getArg(index) != nullptr; }

    Arg *getArg(int index);

	void addArgFlag(std::unique_ptr<Arg> A);
};

} // namespace opt
} // namespace scc

#endif // SCC_OPTION_ARGS_H
