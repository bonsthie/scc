#include "scc/Frontend/LangOpt.h"
#include "scc/ADT/TableBuilder.h"
#include "scc/Frontend/CC1Args.h"
#include "scc/String/StringSwitch.h"
#include <expected>

using namespace scc;

static constexpr LangOpt OptC89 = {
    .trigraphsEnable = true,
    .implicit_intEnable = true,
};

static constexpr LangOpt OptC99 = {
    .trigraphsWarning = true,
    .implicit_intWarning = true,
};

static constexpr LangOpt OptGNU89 = {
    .trigraphsWarning = true,
    .implicit_intWarning = true,
};

static constexpr LangOpt OptGNU99 = {
    .trigraphsWarning = true,
    .implicit_intWarning = true,
};

static constexpr auto VersionTable = //
    TableBuilder<LangVersion, LangOpt, static_cast<size_t>(LangVersion::SIZE)>{}
        .add(LangVersion::C89, OptC89)
        .add(LangVersion::C99, OptC99)
        .add(LangVersion::GNU89, OptGNU89)
        .add(LangVersion::GNU99, OptGNU99)
        .build();

std::expected<LangOpt, err::Code> LangOptBuilder::build() {
    LangVersion Version = getCVersion();
    if (Version == LangVersion::UNKONW)
        return std::unexpected(err::Code::ERROR);

    // TODO: add flags -W option like -Wnocrampte -Wcrampte
    LangOpt Opts = VersionTable[static_cast<size_t>(Version)];
    addWarnindAndFeatureFlags(Opts);
    return Opts;
}

LangVersion LangOptBuilder::getCVersion() {
    Arg *STD = Args.getArg(Opt_std);
    if (!STD)
        return LangDefault;

    LangVersion LV = StringSwitch<LangVersion>(STD->getValue())
                         .caseValue("c89", LangVersion::C89)
                         .caseValue("gnu89", LangVersion::GNU89)
                         .caseValue("gnu99", LangVersion::GNU99)
                         .caseValue("c99", LangVersion::C99)
                         .defaultValue(LangVersion::UNKONW);

    if (LV == LangVersion::UNKONW) {
        const std::string &STDValue = STD->getValue();
        EM.report(err::error)
            .msg("invalid value '")
            .msg(STDValue)
            .msg("' in '--std=")
            .msg(STDValue)
            .msg("'");
    }

    return LV;
}

void LangOptBuilder::addWarnindAndFeatureFlags(LangOpt &Opts) {
    for (const auto &Occurrence : Args.getOccurrences()) {
        switch (Occurrence.getType()) {

#define ALIAS_FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)
#define FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)
#define WARN_FLAG(Enum, FlagType, Desc, Hidden)                                                    \
    case Opt_Wno##Enum:                                                                            \
        Opts.Enum##Warning = false;                                                                \
        break;                                                                                     \
    case Opt_W##Enum:                                                                              \
        Opts.Enum##Warning = true;                                                                 \
        break;

#define F_FLAG(Enum, FlagType, Desc, Hidden)                                                       \
    case Opt_fno##Enum:                                                                            \
        Opts.Enum##Enable = false;                                                                 \
        break;                                                                                     \
    case Opt_f##Enum:                                                                              \
        Opts.Enum##Enable = true;                                                                  \
        break;

#include "scc/Frontend/CC1Args.def"
        }
    }
}
