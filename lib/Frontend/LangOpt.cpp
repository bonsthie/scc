#include "scc/Frontend/LangOpt.h"
#include "scc/Frontend/CC1Args.h"
#include "scc/String/StringSwitch.h"
#include <array>
#include <expected>

using namespace scc;

namespace {

template <typename T, size_t N>
struct TableBuilder {
    std::array<T, N>        data{};
    constexpr TableBuilder &add(LangVersion ver, T opt) {
        data[static_cast<size_t>(ver)] = opt;
        return *this;
    }
    constexpr std::array<T, N> build() { return data; }
};

} // namespace

static constexpr LangOpt OptC89 = {
    .TrigraphEnable = true,
    .TypeDefaultToINT = true,
};

static constexpr LangOpt OptC99 = {};

static constexpr LangOpt OptGNU89 = {};

static constexpr LangOpt OptGNU99 = {};

static constexpr auto VersionTable = //
    TableBuilder<LangOpt, static_cast<size_t>(LangVersion::SIZE)>{}
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
    return VersionTable[static_cast<size_t>(Version)];
}

LangVersion LangOptBuilder::getCVersion() {
    Arg *STD = Args.getArg(Opt_std);
    if (!STD)
        return LangDefault;

    LangVersion LV = StringSwitch<LangVersion>(STD->getValue())
                         .Case("c89", LangVersion::C89)
                         .Case("gnu89", LangVersion::GNU89)
                         .Case("gnu99", LangVersion::GNU99)
                         .Case("c99", LangVersion::C99)
                         .Default(LangVersion::UNKONW);

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
