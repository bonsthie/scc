#include <gtest/gtest.h>

#include "scc/ADT/vector.h"
#include "scc/PreProcessor/PreProcessor.h"

#include "scc/FileManager/FileFinder.h"
#include "scc/FileManager/FileManager.h"
#include "scc/Frontend/FrontendErrorManager.h"
#include "scc/String/StringInterner.h"
#include "scc/Token/Token.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <string_view>

using namespace scc;

namespace {
class TempDir {
  public:
    TempDir() {
        auto               base = std::filesystem::temp_directory_path();
        std::random_device rd;
        Path = base / ("scc_pp_" + std::to_string(rd()));
        std::filesystem::create_directories(Path);
    }

    ~TempDir() {
        std::error_code ec;
        std::filesystem::remove_all(Path, ec);
    }

    const std::filesystem::path &path() const { return Path; }

    std::filesystem::path makeFile(const std::string &Name, std::string_view Contents) {
        auto          FilePath = Path / Name;
        std::ofstream OS(FilePath);
        OS << Contents;
        OS.close();
        return FilePath;
    }

  private:
    std::filesystem::path Path;
};

scc::Vector<tok::TokenKind> lexAllTokens(PreProcessor &PP) {
    scc::Vector<tok::TokenKind> Kinds;
    Token                       Tok;

    while (true) {
        bool ShouldStop = PP.next(Tok);
        Kinds.pushBack(Tok.getTokenKind());
        if (Tok.is(tok::eof))
            break;
        EXPECT_FALSE(ShouldStop);
    }

    return Kinds;
}

File *openFile(FileManager &FM, const std::filesystem::path &Path) {
    File *F = FM.getFile(Path.string());
    EXPECT_NE(F, nullptr);
    return F;
}

} // namespace

TEST(PreProcessorTest, HandlesLocalIncludes) {
    TempDir Tmp;
    auto    MainPath = Tmp.makeFile("main.c", "#include \"header.h\"\nint outer;\n");
    Tmp.makeFile("header.h", "int inner;\n");

    FrontendErrorManager EM;
    FileFinder     Finder({});
    FileManager    FM(Finder, EM);
    BumpAllocator  Arena;
    StringInterner SI(Arena);

    File *MainFile = openFile(FM, MainPath);
    ASSERT_NE(MainFile, nullptr);

    PreProcessor PP(*MainFile, EM, FM, SI);
    auto         Kinds = lexAllTokens(PP);

    scc::Vector<tok::TokenKind> Expected = {tok::t_int, tok::identifier, tok::semi, // from header
                                            tok::t_int, tok::identifier, tok::semi, tok::eof};

    EXPECT_EQ(Kinds, Expected);
    EXPECT_TRUE(EM.getErrsList().empty());
}

TEST(PreProcessorTest, ReportsMissingInclude) {
    TempDir Tmp;
    auto    MainPath = Tmp.makeFile("main.c", "#include \"missing.h\"\n");

    FrontendErrorManager EM;
    FileFinder     Finder({});
    FileManager    FM(Finder, EM);
    BumpAllocator  Arena;
    StringInterner SI(Arena);

    File *MainFile = openFile(FM, MainPath);
    ASSERT_NE(MainFile, nullptr);

    PreProcessor PP(*MainFile, EM, FM, SI);
    Token        Tok;

    EXPECT_TRUE(PP.next(Tok));
    EXPECT_FALSE(EM.getErrsList().empty());
}
