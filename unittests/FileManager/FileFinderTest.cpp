#include "scc/FileManager/FileFinder.h"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include "FMUtils.h"

namespace fs = std::filesystem;
using namespace scc;

class FileFinderTest : public ::testing::Test {
  protected:
    fs::path                    tempDir;
    fs::path                    oldDir;
    std::unique_ptr<FileFinder> FF;

    void SetUp() override {
        oldDir = fs::current_path();

        // Use a unique path to avoid collisions across runs
        tempDir = MakeUniqueTempDir("filefinder");
        std::error_code ec;
        fs::create_directories(tempDir, ec);
        ASSERT_FALSE(ec) << ec.message();

        // Layout:
        // tempDir/
        //   foo.h
        //   foo/{foo.h, foo1.h, foo2.h}
        //   bar/{bar1.h, bar2.h}
        fs::create_directories(tempDir / "foo", ec);
        ASSERT_FALSE(ec) << ec.message();
        fs::create_directories(tempDir / "bar", ec);
        ASSERT_FALSE(ec) << ec.message();

        write_file(tempDir / "foo.h", "// foo header\n");
        write_file(tempDir / "foo" / "foo.h", "// foo/foo.h header\n");
        write_file(tempDir / "foo" / "foo1.h", "// foo/foo1.h header\n");
        write_file(tempDir / "foo" / "foo2.h", "// foo/foo2.h header\n");
        write_file(tempDir / "bar" / "bar1.h", "// bar/bar1 header\n");
        write_file(tempDir / "bar" / "bar2.h", "// bar/bar2 header\n");

        fs::current_path(tempDir, ec);
        ASSERT_FALSE(ec) << ec.message();
        ASSERT_EQ(fs::current_path(), tempDir);

        // System include path points to tempDir/foo
        std::vector<std::string> paths = {(tempDir / "foo").string()};
        FF = std::make_unique<FileFinder>(paths);
    }

    void TearDown() override {
        std::error_code ec;
        fs::current_path(oldDir, ec);
        fs::remove_all(tempDir, ec);
    }
};

TEST_F(FileFinderTest, FileFromRoot) {
    FileID *id = FF->getFileID("foo.h");
    ASSERT_NE(id, nullptr);
    EXPECT_EQ(id->getName(), tempDir / "foo.h");
}

TEST_F(FileFinderTest, FileFromRootDoesntExist) {
    EXPECT_EQ(FF->getFileID("DoesntExist.h"), nullptr);
}

TEST_F(FileFinderTest, FileFromValidSystemPath) {
    FileID *id = FF->getSystemFileID("foo1.h");
    ASSERT_NE(id, nullptr);
    EXPECT_EQ(id->getName(), tempDir / "foo" / "foo1.h");
}

TEST_F(FileFinderTest, FileFromValidSystemPathDoesntExist) {
    EXPECT_EQ(FF->getSystemFileID("DoesntExist.h"), nullptr);
}

TEST_F(FileFinderTest, SystemAndRelativeResolveToSameObject) {
    FileID *fromSys = FF->getSystemFileID("foo.h");
    FileID *fromRoot = FF->getFileID("foo/foo.h");
    ASSERT_NE(fromSys, nullptr);
    ASSERT_NE(fromRoot, nullptr);

    // Cached object identity (same FileID object) is expected
    EXPECT_EQ(fromSys, fromRoot);
}

TEST_F(FileFinderTest, ResolveSiblingViaBaseFileID) {
    FileID *base = FF->getFileID("bar/bar1.h");
    ASSERT_NE(base, nullptr);

    FileID *sib = FF->getFileID("bar2.h", *base);
    ASSERT_NE(sib, nullptr);
    EXPECT_EQ(sib->getName(), tempDir / "bar" / "bar2.h");
}

TEST_F(FileFinderTest, EmptyStringName) { EXPECT_EQ(FF->getFileID(""), nullptr); }

TEST_F(FileFinderTest, EmptyStringNameSys) { EXPECT_EQ(FF->getSystemFileID(""), nullptr); }

TEST_F(FileFinderTest, CachesSamePathToSamePointer) {
    FileID *a = FF->getFileID("foo/foo2.h");
    FileID *b = FF->getFileID("foo/foo2.h");
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(a, b) << "Expect cached FileID for same absolute path";
}

TEST_F(FileFinderTest, RelativeTraversalIsHandled) {
    EXPECT_NE(FF->getFileID("foo/../foo.h"), nullptr);
}

TEST_F(FileFinderTest, AbsolutePathLookup) {
    // Calling with absolute path should work (if you support it)
    fs::path abs = tempDir / "bar" / "bar2.h";
    FileID  *id = FF->getFileID(abs.string());
    ASSERT_NE(id, nullptr);
    EXPECT_EQ(id->getName(), tempDir / "bar" / "bar2.h");
}
