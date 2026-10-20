#include "gtest/gtest.h"

#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/Parser/ParserErrorManager.h"

using namespace scc;

TEST(ParserErrorManagerTest, ExpectedXListsTheFinalAlternativeWithOr) {
    FrontendErrorManager FrontendErrors;
    ParserErrorManager   ParserErrors(FrontendErrors);

    const auto &Diag = ParserErrors.expectedX(PosView{}, tok::identifier, tok::l_paren,
                                               tok::l_square, tok::star);

    EXPECT_EQ("expected identifier, '(', '[' or '*'", Diag.getMsg());
}
