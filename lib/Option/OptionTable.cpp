#include "scc/Option/OptionTable.h"
#include "scc/Option/Args.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string_view>

using namespace scc;
using namespace scc::opt;

ArgsList *OptionTable::parseArgs(const std::vector<const char *> &argv) {
    ArgsList *Args = new ArgsList;
    if (Args == nullptr)
        return nullptr;

    ArgvIt it = argv.begin();
    ArgvIt end = argv.end();

    while (it != end) {
        std::unique_ptr<Arg> a = nextArg(it, end);
        if (a)
            Args->addArgFlag(std::move(a));
        else
            std::cout << "nooop : " << *it << std::endl;
		if (it != end)
			it++;
    }
    return Args;
}

std::unique_ptr<Arg> OptionTable::nextArg(ArgvIt &it, ArgvIt end) {
    std::span<const OptionSpec> Options = specs();

    std::string_view str(*it);
    for (auto Opt : Options) {
        switch (Opt.kind) {
        case OptKind::Joined:
        case OptKind::Equal:
            if (str.starts_with(Opt.spelling) == true) {
                std::string subStr(str.substr(strlen(Opt.spelling)));
                return std::make_unique<Arg>((int)Opt.key, (Arg::valueType)Opt.vtype, subStr);
            }
            break;

        case OptKind::Separate:
            if (str.compare(Opt.spelling) == 0) {

                it++;
                if (it == end)
                    return nullptr;
                return std::make_unique<Arg>((int)Opt.key, (Arg::valueType)Opt.vtype, *it);
            }
            break;

        case OptKind::Flag:
            if (str.compare(Opt.spelling) == 0)
                return std::make_unique<Arg>((int)Opt.key);
            break;
        default:
            continue;
        }
    }
    return nullptr;
}

void OptionTable::printOpt(std::ostream &O) {
    std::span<const OptionSpec> opt = specs();

    for (const auto &Option : opt) {
        std::string fullSpelling = Option.spelling + printHasOption(Option.kind);
        O << " " << std::left << std::setw(20) << fullSpelling << Option.help << "\n";
    }
}

std::string OptionTable::printHasOption(OptKind kind) {
    switch (kind) {
    case OptKind::Joined:
    case OptKind::Equal:
        return "<arg>";
    case OptKind::Separate:
    case OptKind::JoinedOrSeparate:
        return " <arg>";
    default:
        return "";
    }
}
