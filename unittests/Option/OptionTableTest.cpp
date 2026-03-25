// File: SccOptionTableTest.cpp
#include <gtest/gtest.h>
#include <string>

#include "scc/ADT/vector.h"

#include "scc/Error/ErrorManager.h"
#include "scc/Option/Args.h"
#include "scc/Option/OptionTable.h"

using namespace scc;

enum SccOptionIndex { Opt_none, Opt_test, Opt_oui, Opt_L, Opt_I };

class SccOptionTable : public OptionTable {
    static constexpr OptionSpec opt[] = {
        {Opt_test, "-test", OptKind::Flag, ValType::None, "this is a test", false},
        {Opt_oui, "--oui=", OptKind::Equal, ValType::Str, "oui oui baguette", false},
        {Opt_L, "-L", OptKind::Separate, ValType::StrList, "L is for losser", false},
        {Opt_I, "-I", OptKind::JoinedOrSeparate, ValType::StrList, "system include folder", false},
    };

  public:
    explicit SccOptionTable(ErrorManager &EM) : OptionTable(EM) {}
    std::span<const OptionSpec> specs() const override { return opt; }
};

// Convenience: build argv vector (already skipping argv[0])
static scc::vector<const char *> mkArgv(std::initializer_list<const char *> args) {
    return scc::vector<const char *>{args};
}

TEST(SccOptionTable, ParsesFlagTest) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-test"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    EXPECT_EQ(args->size(), 1u);
    ASSERT_NE(args->getArg(Opt_test), nullptr);

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, ParsesEqualOption) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"--oui=baguette"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    EXPECT_EQ(args->size(), 1u);
    ASSERT_NE(args->getArg(Opt_oui), nullptr);

    auto *A = args->getArg(Opt_oui);
    ASSERT_FALSE(A->empty());
    EXPECT_EQ(A->getValue(), "baguette");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, ParsesSeparateOption) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-L", "lib"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    EXPECT_EQ(args->size(), 1u);
    ASSERT_NE(args->getArg(Opt_L), nullptr);
    EXPECT_EQ(args->getArg(Opt_L)->count(), 1u);

    auto *A = args->getArg(Opt_L);
    ASSERT_FALSE(A->empty());
    ASSERT_EQ(A->getValuesList().size(), 1u);
    EXPECT_EQ(A->getValuesList()[0], "lib");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, ParsesJoinedOrSeparate_I_Joined) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-I/usr/include", "-I/opt/include"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_I), nullptr);
    EXPECT_EQ(args->getArg(Opt_I)->count(), 2u);

    auto *A = args->getArg(Opt_I);
    ASSERT_EQ(A->getValuesList().size(), 2u);
    EXPECT_EQ(A->getValuesList()[0], "/usr/include");
    EXPECT_EQ(A->getValuesList()[1], "/opt/include");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, ParsesJoinedOrSeparate_I_Separate) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-I", "inc1", "-I", "inc2"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_I), nullptr);
    EXPECT_EQ(args->getArg(Opt_I)->count(), 2u);

    auto *A = args->getArg(Opt_I);
    ASSERT_EQ(A->getValuesList().size(), 2u);
    EXPECT_EQ(A->getValuesList()[0], "inc1");
    EXPECT_EQ(A->getValuesList()[1], "inc2");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, SeparateMissingArgumentEmitsWarning) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-L"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    // -L should not be recorded because the value is missing
    EXPECT_EQ(args->getArg(Opt_L), nullptr);

    // One (or more) diagnostics expected
    const auto &errs = EM.getErrsList();
    ASSERT_FALSE(errs.empty());
    EXPECT_EQ(errs.front()->getDiagLevel(), err::DiagLevel::error);
    EXPECT_NE(errs.front()->getMsg().find("-L"), std::string::npos);
    EXPECT_NE(errs.front()->getMsg().find("missing"), std::string::npos);
}

TEST(SccOptionTable, SeparateValueThatLooksLikeFlag_EmitsWarning) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-L", "-foo"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    // Still consider it missing (parser rejects values starting with '-')
    EXPECT_EQ(args->getArg(Opt_L), nullptr);

    const auto &errs = EM.getErrsList();
    ASSERT_FALSE(errs.empty());
    EXPECT_EQ(errs.front()->getDiagLevel(), err::DiagLevel::error);
    EXPECT_NE(errs.front()->getMsg().find("-L"), std::string::npos);
    EXPECT_NE(errs.front()->getMsg().find("missing"), std::string::npos);
}

TEST(SccOptionTable, MixedAllOptions) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-test", "--oui=croissant", "-L", "lib", "-Iinc1", "-I", "inc2"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);

    // -test
    ASSERT_NE(args->getArg(Opt_test), nullptr);
    EXPECT_EQ(args->getArg(Opt_test)->count(), 0);
    EXPECT_TRUE(args->getArg(Opt_test)->empty());

    // --oui=
    ASSERT_NE(args->getArg(Opt_oui), nullptr);
    EXPECT_EQ(args->getArg(Opt_oui)->count(), 1u);
    EXPECT_EQ(args->getArg(Opt_oui)->getValue(), "croissant");

    // -L
    ASSERT_NE(args->getArg(Opt_L), nullptr);
    EXPECT_EQ(args->getArg(Opt_L)->count(), 1u);
    ASSERT_EQ(args->getArg(Opt_L)->getValuesList().size(), 1u);
    EXPECT_EQ(args->getArg(Opt_L)->getValuesList()[0], "lib");

    // -I (joined + separate)
    ASSERT_NE(args->getArg(Opt_I), nullptr);
    EXPECT_EQ(args->getArg(Opt_I)->count(), 2u);
    ASSERT_EQ(args->getArg(Opt_I)->getValuesList().size(), 2u);
    EXPECT_EQ(args->getArg(Opt_I)->getValuesList()[0], "inc1");
    EXPECT_EQ(args->getArg(Opt_I)->getValuesList()[1], "inc2");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, I_MissingThenSeparateValue) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    // First -I missing, second -I has "test"
    auto argv = mkArgv({"-I", "-I", "test"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);

    // One diagnostic for the first missing value
    const auto &errs = EM.getErrsList();
    ASSERT_FALSE(errs.empty());
    EXPECT_EQ(errs.front()->getDiagLevel(), err::DiagLevel::error);
    EXPECT_NE(errs.front()->getMsg().find("-I"), std::string::npos);
    EXPECT_NE(errs.front()->getMsg().find("missing"), std::string::npos);

    // Only the second -I contributes a value
    ASSERT_NE(args->getArg(Opt_I), nullptr);
    EXPECT_EQ(args->getArg(Opt_I)->count(), 1u);
    ASSERT_EQ(args->getArg(Opt_I)->getValuesList().size(), 1u);
    EXPECT_EQ(args->getArg(Opt_I)->getValuesList()[0], "test");
}

TEST(SccOptionTable, Oui_LastWins) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"--oui=baguette", "--oui=crampte"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_oui), nullptr);

    EXPECT_EQ(args->getArg(Opt_oui)->getValue(), "crampte");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, FlagRepeatCounts) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-test", "-test"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_test), nullptr);

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, I_OrderPreserved_JoinedThenSeparateThenJoined) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-Iinc1", "-I", "inc2", "-Iinc3"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_I), nullptr);
    EXPECT_EQ(args->getArg(Opt_I)->count(), 3u);

    const auto &vals = args->getArg(Opt_I)->getValuesList();
    ASSERT_EQ(vals.size(), 3u);
    EXPECT_EQ(vals[0], "inc1");
    EXPECT_EQ(vals[1], "inc2");
    EXPECT_EQ(vals[2], "inc3");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, L_MultipleValuesAccumulateInOrder) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-L", "lib1", "-L", "lib2"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    ASSERT_NE(args->getArg(Opt_L), nullptr);
    EXPECT_EQ(args->getArg(Opt_L)->count(), 2u);

    const auto &vals = args->getArg(Opt_L)->getValuesList();
    ASSERT_EQ(vals.size(), 2u);
    EXPECT_EQ(vals[0], "lib1");
    EXPECT_EQ(vals[1], "lib2");

    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(SccOptionTable, I_MissingArgumentEmitsError) {
    ErrorManager   EM;
    SccOptionTable Opt(EM);

    auto argv = mkArgv({"-I"});
    auto args = Opt.parseArgs(argv);

    ASSERT_TRUE(args);
    EXPECT_EQ(args->getArg(Opt_I), nullptr);

    const auto &errs = EM.getErrsList();
    ASSERT_FALSE(errs.empty());
    EXPECT_EQ(errs.front()->getDiagLevel(), err::DiagLevel::error);
    EXPECT_NE(errs.front()->getMsg().find("-I"), std::string::npos);
    EXPECT_NE(errs.front()->getMsg().find("missing"), std::string::npos);
}
