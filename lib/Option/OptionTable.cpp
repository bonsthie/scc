#include "scc/Option/OptionTable.h"
#include "scc/Error/Error.h"
#include "scc/Option/Args.h"
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string_view>

using namespace scc;

ArgsList *OptionTable::parseArgs(const scc::vector<const char *> &argv) {
    auto   Args = new ArgsList;
    ArgvIt it = argv.begin(), end = argv.end();

    while (it != end) {
        if (auto a = nextArg(it, end)) {
            Args->addArgFlag(std::move(a));
        } else {
            ++it;
        }
    }
    return Args;
}

static inline bool starts_with(std::string_view s, std::string_view pfx) {
    return s.size() >= pfx.size() && s.compare(0, pfx.size(), pfx) == 0;
}

std::unique_ptr<Arg> OptionTable::nextArg(ArgvIt &it, ArgvIt end) {
    std::span<const OptionSpec> Options = specs();

    std::string_view tok = (*it ? std::string_view(*it) : std::string_view());
    for (const auto &Opt : Options) {
        std::string_view spell = Opt.spelling ? std::string_view(Opt.spelling) : std::string_view();

        switch (Opt.kind) {
        case OptKind::JoinedOrSeparate:
        case OptKind::Joined: {
            // e.g., "-I/usr/include" where spelling is "-I"
            if (starts_with(tok, spell) && tok.size() > spell.size()) {
                std::string val(tok.substr(spell.size()));
                ++it; // consumed current token
                return std::make_unique<Arg>((int)Opt.key, (Arg::valueType)Opt.vtype,
                                             std::move(val));
            }
            if (Opt.kind == OptKind::Joined)
                return nullptr;
            // fall back on separate
            [[fallthrough]];
        }
        case OptKind::Separate: {
            // e.g., "-o output" where spelling is "-o"
            if (tok == spell) {
                auto next = std::next(it);
                if (next == end) {
                    EM.report(err::error).msg(std::string(spell)).msg(" missing argument");
                    return nullptr;
                }
                std::string_view val_sv(*next ? *next : "");
                if (!val_sv.empty() && val_sv.front() == '-') {
                    EM.report(err::error).msg(std::string(spell)).msg(" missing argument");
                    return nullptr;
                }
                std::string val(val_sv);
                it = std::next(next);
                return std::make_unique<Arg>((int)Opt.key, (Arg::valueType)Opt.vtype,
                                             std::move(val));
            }
            break;
        }

        case OptKind::Equal: {
            // e.g., "--opt=value" where spelling is "--opt="
            if (starts_with(tok, spell) && tok.size() > spell.size()) {
                std::string val(tok.substr(spell.size()));
                ++it;
                return std::make_unique<Arg>((int)Opt.key, (Arg::valueType)Opt.vtype,
                                             std::move(val));
            }
            break;
        }
        case OptKind::Flag: {
            if (tok == spell) {
                ++it;
                return std::make_unique<Arg>((int)Opt.key);
            }
            break;
        }
        }
    }

    EM.report(err::error).msg("unknow flag '").msg(std::string(tok)).msg("'");
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
