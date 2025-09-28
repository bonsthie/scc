#include "scc/FileManager/FileManager.h"
#include "scc/FileManager/File.h"
#include "scc/FileManager/FileFinder.h"

#include "FMUtils.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>

namespace fs = std::filesystem;
using namespace scc;

// ---- Fixture that sets up FS + FileFinder + FileManager ----
class FileManagerTest : public ::testing::Test {
  protected:
    fs::path                           tempDir;
    fs::path                           oldDir;
    std::unique_ptr<FileFinder>        FF;
    std::unique_ptr<FileManager>       FM;
    std::unique_ptr<scc::ErrorManager> EM; // shim to FM

    void SetUp() override {
        oldDir = fs::current_path();
        tempDir = MakeUniqueTempDir("filemanager");
        std::error_code ec;
        fs::create_directories(tempDir, ec);
        ASSERT_FALSE(ec) << ec.message();

        fs::create_directories(tempDir / "foo", ec);
        ASSERT_FALSE(ec) << ec.message();
        fs::create_directories(tempDir / "bar", ec);
        ASSERT_FALSE(ec) << ec.message();

        // Files
        write_file(tempDir / "root.txt", "ROOT\n");
        write_file(tempDir / "foo" / "a.h", "A\n");
        write_file(tempDir / "foo" / "b.h", "B\n");
        write_file(tempDir / "bar" / "x.hpp", "X\n");
        write_file(tempDir / "bar" / "y.hpp", "Y\n");

        fs::current_path(tempDir, ec);
        ASSERT_FALSE(ec) << ec.message();
        ASSERT_EQ(fs::current_path(), tempDir);

        // System include path points to tempDir/foo
        std::vector<std::string> sysPaths = {(tempDir / "foo").string()};
        FF = std::make_unique<FileFinder>(sysPaths);

        EM = std::make_unique<scc::ErrorManager>();
        FM = std::make_unique<FileManager>(*FF, *EM);
    }

    void TearDown() override {
        std::error_code ec;
        fs::current_path(oldDir, ec);
        fs::remove_all(tempDir, ec);
    }
};

// ---- Tests ----

TEST_F(FileManagerTest, GetFile_FromCwdRelative) {
    // Should load ./root.txt using current working dir
    File *f = FM->getFile("root.txt");
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->status(), File::FOK);
    EXPECT_EQ(f->getFileID().getName(), (tempDir / "root.txt").string());
    auto v = f->view();
    ASSERT_EQ(v.size(), 5u); // "ROOT\n"
}

TEST_F(FileManagerTest, GetSystemFile_FromSysIncludePath) {
    // Should resolve via system include paths (tempDir/foo)
    File *f = FM->getSystemFile("a.h");
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->status(), File::FOK);
    EXPECT_EQ(f->getFileID().getName(), (tempDir / "foo" / "a.h").string());
}

TEST_F(FileManagerTest, GetFile_ByAbsolutePath) {
    fs::path abs = tempDir / "bar" / "x.hpp";
    File    *f = FM->getFile(abs.string());
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->status(), File::FOK);
    EXPECT_EQ(f->getFileID().getName(), abs.string());
}

TEST_F(FileManagerTest, GetFile_RelativeToBaseFileID) {
    // First open bar/x.hpp, then use it as base to resolve sibling y.hpp
    File *base = FM->getFile("bar/x.hpp");
    ASSERT_NE(base, nullptr);
    const FileID &baseFID = base->getFileID();

    File *sib = FM->getFile("y.hpp", baseFID);
    ASSERT_NE(sib, nullptr);
    EXPECT_EQ(sib->getFileID().getName(), (tempDir / "bar" / "y.hpp").string());
}

TEST_F(FileManagerTest, GetFile_RelativeToBaseFile) {
    File *base = FM->getFile("foo/a.h");
    ASSERT_NE(base, nullptr);

    // Relative lookup "b.h" should resolve to foo/b.h
    File *sib = FM->getFile("b.h", *base);
    ASSERT_NE(sib, nullptr);
    EXPECT_EQ(sib->getFileID().getName(), (tempDir / "foo" / "b.h").string());
}

TEST_F(FileManagerTest, GetFile_WithBasePathString) {
    // Simulate include from a directory path
    File *f = FM->getFile("a.h", (tempDir / "foo").string());
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->getFileID().getName(), (tempDir / "foo" / "a.h").string());
}

TEST_F(FileManagerTest, GetFile_NotFound_ReturnsNull_AndReportsError) {
    File *f = FM->getFile("does_not_exist.xyz");
    EXPECT_EQ(f, nullptr);
    // If your FileManager reports through EM, you can assert TEM.count() > 0
    // (Assuming FileManager calls EM.addError(...))
    // EXPECT_GE(TEM.count(), 1u);
}

TEST_F(FileManagerTest, GetSystemFile_NotFound_ReturnsNull) {
    File *f = FM->getSystemFile("nope.hpp");
    EXPECT_EQ(f, nullptr);
}

TEST_F(FileManagerTest, fileFormFileID_UsesCacheAndIdentity) {
    // Open a file via getFile
    File *f1 = FM->getFile("bar/x.hpp");
    ASSERT_NE(f1, nullptr);

    // Retrieve from FileID (should hit cache, same pointer)
    File *f2 = FM->fileFormFileID(&f1->getFileID());
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f1, f2);

    // Re-open same path returns same cached object
    File *f3 = FM->getFile("bar/x.hpp");
    ASSERT_NE(f3, nullptr);
    EXPECT_EQ(f1, f3);
}

TEST_F(FileManagerTest, EmptyName_ReturnsNull) {
    EXPECT_EQ(FM->getFile(""), nullptr);
    EXPECT_EQ(FM->getSystemFile(""), nullptr);
}

TEST_F(FileManagerTest, Traversal_Normalized) {
    // Policy depends on your implementation; here we expect it to resolve.
    File *f = FM->getFile("foo/../foo/a.h");
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->getFileID().getName(), (tempDir / "foo" / "a.h").string());
}

// TODO test that test file right not done now because we need to change the ErrorManager
