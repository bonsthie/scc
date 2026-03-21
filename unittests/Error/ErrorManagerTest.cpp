#include <gtest/gtest.h>

#include "scc/Error/Error.h"
#include "scc/Error/ErrorManager.h"

#include <memory>
#include <sstream>
#include <string>

using namespace scc;

namespace {
class HaltingError : public Error {
  public:
    explicit HaltingError(err::DiagLevel Level) : Error(Level) {}
    err::EmitionLeveL shouldEmit() const override {
        return static_cast<err::EmitionLeveL>(err::Emit | err::Stop);
    }
};
}

TEST(ErrorManagerTest, ReportsAndClearsDiagnostics) {
    std::ostringstream Out;
    ErrorManager       EM(ErrorManager::defaultFactory(), Out);

    Error &E = EM.report(err::error);
    E.msg("something bad");

    ASSERT_EQ(EM.size(), 1);
    EXPECT_FALSE(EM.emit());
    EXPECT_EQ(EM.size(), 0);
    EXPECT_NE(Out.str().find("error"), std::string::npos);
    EXPECT_NE(Out.str().find("something bad"), std::string::npos);
}

TEST(ErrorManagerTest, CustomFactoryControlsEmission) {
    std::ostringstream Out;
    ErrorManager       EM(
        [](err::DiagLevel Level) { return std::make_unique<HaltingError>(Level); }, Out);

    Error &E = EM.report(err::warning);
    E.msg("stop here");

    EXPECT_TRUE(EM.emit());
    EXPECT_NE(Out.str().find("warning"), std::string::npos);
}
