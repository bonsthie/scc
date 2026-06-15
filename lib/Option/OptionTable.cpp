#include "scc/Option/OptionTable.h"
#include "scc/Error/Error.h"
#include "scc/Option/Args.h"
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

using namespace scc;

ArgsList *OptionTable::parseArgs(const scc::Vector<const char *> &Argv) {
    auto *   Args = new ArgsList;
    ArgvIt It = Argv.begin(), End = Argv.end();

    while (It != End) {
        std::string_view Tok = (*It ? std::string_view(*It) : std::string_view());
        if (!Tok.empty() && Tok[0] != '-') {
            Args->addFile(Tok);
            ++It;
            continue;
        }

        std::string MatchedSpelling;
        if (auto a = nextArg(It, End, &MatchedSpelling)) {
            Args->addArgFlag(std::move(a), MatchedSpelling);
        } else {
            ++It;
        }
    }
    return Args;
}

static inline bool starts_with(std::string_view s, std::string_view Pfx) {
    return s.size() >= Pfx.size() && s.compare(0, Pfx.size(), Pfx) == 0;
}

static const OptionSpec *find_visible_option_for_alias(std::span<const OptionSpec> Options,
                                                   const OptionSpec            &Alias) {
    for (const auto &Option : Options) {
        if (!Option.Hidde && Option.Key == Alias.Key)
            return &Option;
    }

    return nullptr;
}

std::unique_ptr<Arg> OptionTable::nextArg(ArgvIt &It, ArgvIt End, std::string *MatchedSpelling) {
    std::span<const OptionSpec> Options = specs();

    std::string_view Tok = (*It ? std::string_view(*It) : std::string_view());
    for (const auto &Opt : Options) {
        std::string_view Spell = Opt.Spelling ? std::string_view(Opt.Spelling) : std::string_view();

        switch (Opt.Kind) {
        case OptKind::JoinedOrSeparate:
        case OptKind::Joined: {
            // e.g., "-I/usr/include" where spelling is "-I"
            if (starts_with(Tok, Spell) && Tok.size() > Spell.size()) {
                std::string Val(Tok.substr(Spell.size()));
                if (MatchedSpelling)
                    *MatchedSpelling = std::string(Spell);
                ++It; // consumed current token
                return std::make_unique<Arg>((int)Opt.Key, (Arg::ValueType)Opt.Vtype,
                                             std::move(Val));
            }
            if (Opt.Kind == OptKind::Joined)
                return nullptr;
            // fall back on separate
            [[fallthrough]];
        }
        case OptKind::Separate: {
            // e.g., "-o output" where spelling is "-o"
            if (Tok == Spell) {
                const auto *Next = std::next(It);
                if (Next == End) {
                    EM.report(err::error).msg(std::string(Spell)).msg(" missing argument");
                    return nullptr;
                }
                std::string_view ValSv(*Next ? *Next : "");
                if (!ValSv.empty() && ValSv.front() == '-') {
                    EM.report(err::error).msg(std::string(Spell)).msg(" missing argument");
                    return nullptr;
                }
                std::string Val(ValSv);
                if (MatchedSpelling)
                    *MatchedSpelling = std::string(Spell);
                It = std::next(Next);
                return std::make_unique<Arg>((int)Opt.Key, (Arg::ValueType)Opt.Vtype,
                                             std::move(Val));
            }
            break;
        }

        case OptKind::Equal: {
            // e.g., "--opt=value" where spelling is "--opt="
            if (starts_with(Tok, Spell) && Tok.size() > Spell.size()) {
                std::string Val(Tok.substr(Spell.size()));
                if (MatchedSpelling)
                    *MatchedSpelling = std::string(Spell);
                ++It;
                return std::make_unique<Arg>((int)Opt.Key, (Arg::ValueType)Opt.Vtype,
                                             std::move(Val));
            }
            break;
        }
        case OptKind::Flag: {
            if (Tok == Spell) {
                if (MatchedSpelling)
                    *MatchedSpelling = std::string(Spell);
                ++It;
                return std::make_unique<Arg>((int)Opt.Key);
            }
            break;
        }
        }
    }

    EM.report(err::error).msg("unknow flag '").msg(std::string(Tok)).msg("'");
    return nullptr;
}

void OptionTable::printOpt(std::ostream &O) {
    std::span<const OptionSpec> Opt = specs();

    for (const auto &Option : Opt) {
        std::string FullSpelling = Option.Spelling + printHasOption(Option.Kind);

        if (Option.Hidde) {
            const OptionSpec *Base = find_visible_option_for_alias(Opt, Option);
            if (!Base)
                continue;

            std::string Target = Base->Spelling + printHasOption(Base->Kind);
            O << " " << std::left << std::setw(20) << FullSpelling << "alias of " << Target
              << "\n";
            continue;
        }

        std::string Help = Option.Help ? std::string(Option.Help) : std::string();
        O << " " << std::left << std::setw(20) << FullSpelling << Help << "\n";
    }
}

std::string OptionTable::printHasOption(OptKind Kind) {
    switch (Kind) {
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
