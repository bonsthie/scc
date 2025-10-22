#include "scc/FileManager/MemoryBufferView.h"
#include "scc/Lex/FileLexer.h"
#include "scc/Token/Token.h"
#include <gtest/gtest.h>
#include <memory>

using namespace scc;

class FileLexTests : public ::testing::Test {
  public:
    std::unique_ptr<FileID> FID;

    void SetUp() override { FID = std::make_unique<FileID>("test File", 1); }
};

TEST_F(FileLexTests, BasicTest) {
    std::string      str("int main() { return 0; }");
    MemoryBufferView MV(str.c_str(), str.size());

    FileLexer FL(std::move(MV), *FID);
    Token     TK;

    FL.next(TK);
    EXPECT_EQ(TK.getTokenKind(), tok::t_int);
}

