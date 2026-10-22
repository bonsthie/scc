#include <gtest/gtest.h>

#include "scc/Error/Error.h"
#include "scc/Error/ErrorManager.h"

#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>

using namespace scc;

namespace {
class HaltingError : public Error {
  public:
    explicit HaltingError(err::DiagLevel Level) : Error(Level) {}
    err::EmitionLeveL shouldEmit() const override {
        return static_cast<err::EmitionLeveL>(err::Emit | err::Stop);
    }
};

class ErrorCodeTest : public testing::Test {
  protected:
    std::string             Path;
    std::unique_ptr<FileID> FID;

    void SetUp() override {
        Path = testing::TempDir() + "scc-diagnostic-XXXXXX";
        int FD = mkstemp(Path.data());
        ASSERT_GE(FD, 0);
        close(FD);
        FID = std::make_unique<FileID>(Path, 1);
    }

    void TearDown() override { unlink(Path.c_str()); }

    void writeSource(const std::string &Text) {
        std::ofstream Source(Path, std::ios::binary);
        Source << Text;
        ASSERT_TRUE(Source.good());
    }
};
} // namespace

TEST_F(ErrorCodeTest, PointLocationClearsPreviousTokenRange) {
    writeSource("int foo;\n");
    std::ostringstream Out;
    Error(err::note)
        .at(SourceRange(FID.get(), {1, 1}, {1, 4}))
        .at(PosView{{1, 5}, FID.get()})
        .printCode(Out);
    EXPECT_EQ(Out.str(), "1 | int foo;\n  |     ^\n");
}

TEST_F(ErrorCodeTest, OmitsCodeForMissingSourceAndInvalidLocations) {
    writeSource("int foo;\n");
    std::ostringstream Out;
    Error(err::error).printCode(Out);
    Error(err::error).at(PosView{}).printCode(Out);
    Error(err::error).at(PosView{{0, 1}, FID.get()}).printCode(Out);
    Error(err::error).at(PosView{{1, 0}, FID.get()}).printCode(Out);
    Error(err::error).at(PosView{{1, 100}, FID.get()}).printCode(Out);
    Error(err::error).at(PosView{{3, 1}, FID.get()}).printCode(Out);
    EXPECT_TRUE(Out.str().empty());

    unlink(Path.c_str());
    Error(err::error).at(PosView{{1, 1}, FID.get()}).msg("still printed").print(Out);
    EXPECT_NE(Out.str().find(":1:1: error: still printed\n"), std::string::npos);
    EXPECT_EQ(Out.str().find(" | "), std::string::npos);
}

TEST(ErrorManagerTest, ErrorDiagnosticsEmitAndStop) {
    std::ostringstream Out;
    ErrorManager       EM(ErrorManager::defaultFactory(), Out);

    Error &E = EM.report(err::error);
    E.msg("something bad");

    ASSERT_EQ(EM.size(), 1);
    EXPECT_TRUE(EM.emit());
    EXPECT_EQ(EM.size(), 0);
    EXPECT_NE(Out.str().find("error"), std::string::npos);
    EXPECT_NE(Out.str().find("something bad"), std::string::npos);
}

TEST(ErrorManagerTest, WarningDiagnosticsEmitWithoutStopping) {
    std::ostringstream Out;
    ErrorManager       EM(ErrorManager::defaultFactory(), Out);

    Error &E = EM.report(err::warning);
    E.msg("heads up");

    ASSERT_EQ(EM.size(), 1);
    EXPECT_FALSE(EM.emit());
    EXPECT_EQ(EM.size(), 0);
    EXPECT_NE(Out.str().find("warning"), std::string::npos);
    EXPECT_NE(Out.str().find("heads up"), std::string::npos);
}

TEST(ErrorManagerTest, StopClearsRemainingDiagnostics) {
    std::ostringstream Out;
    ErrorManager       EM(ErrorManager::defaultFactory(), Out);

    EM.report(err::warning).msg("first warning");
    EM.report(err::error).msg("fatal error");
    EM.report(err::warning).msg("should be dropped");

    ASSERT_EQ(EM.size(), 3);
    EXPECT_TRUE(EM.emit());
    EXPECT_EQ(EM.size(), 0);

    const std::string Printed = Out.str();
    EXPECT_NE(Printed.find("first warning"), std::string::npos);
    EXPECT_NE(Printed.find("fatal error"), std::string::npos);
    EXPECT_EQ(Printed.find("should be dropped"), std::string::npos);
}

TEST(ErrorManagerTest, CustomFactoryControlsEmission) {
    std::ostringstream Out;
    ErrorManager EM([](err::DiagLevel Level) { return std::make_unique<HaltingError>(Level); },
                    Out);

    Error &E = EM.report(err::warning);
    E.msg("stop here");

    EXPECT_TRUE(EM.emit());
    EXPECT_NE(Out.str().find("warning"), std::string::npos);
}
