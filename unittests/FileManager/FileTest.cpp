
#include "scc/FileManager/File.h"
#include "scc/FileManager/FileID.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "FMUtils.h"

using namespace scc;

TEST(File, HappyPathSmallText) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "hello.txt";
    {
        std::ofstream(path) << "hello\n";
    }

    FileID fid{path.string(), 0};
    File   f(fid);

    EXPECT_EQ(f.status(), File::FOK);
    EXPECT_EQ(f.size(), 6u);
    auto v = f.view();
    ASSERT_EQ(v.size(), 6u);
    EXPECT_EQ(std::memcmp(v.raw(), "hello\n", 6), 0);
}

TEST(File, ZeroLength) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "empty.bin";
    {
        std::ofstream ofs(path);
    } // zero length

    FileID fid{path.string(), 1};
    File   f(fid);

    EXPECT_EQ(f.status(), File::F_MAPING);
    EXPECT_EQ(f.size(), 0u);
    EXPECT_TRUE(f.view().size() == 0u);
    // data() may be nullptr or valid; assert consistent with your contract
}

TEST(File, NonExistent) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "nope.bin";

    FileID fid{path.string(), 2};
    File   f(fid);

    EXPECT_NE(f.status(), File::FOK);
}

TEST(File, BinaryWithNuls) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "bin.dat";
    {
        std::ofstream ofs(path, std::ios::binary);
        unsigned char bytes[4] = {0x00, 0xFF, 0x7F, 0x00};
        ofs.write(reinterpret_cast<char *>(bytes), 4);
    }
    FileID fid{path.string(), 3};
    File   f(fid);
    ASSERT_EQ(f.status(), File::FOK);
    auto v = f.view();
    ASSERT_EQ(v.size(), 4u);
    const unsigned char *p = reinterpret_cast<const unsigned char *>(v.raw());
    EXPECT_EQ(p[0], 0x00);
    EXPECT_EQ(p[1], 0xFF);
    EXPECT_EQ(p[2], 0x7F);
    EXPECT_EQ(p[3], 0x00);
}

#if defined(__unix__)
#    include <sys/stat.h>
TEST(File, PermissionDenied) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "locked.txt";
    {
        std::ofstream(path) << "x";
    }
    ::chmod(path.c_str(), 0000);
    FileID fid{path.string(), 4};
    File   f(fid);
    EXPECT_NE(f.status(), File::FOK);
    ::chmod(path.c_str(), 0644); // cleanup
}
#endif

TEST(File, MoveSemantics) {
    auto dir = MakeUniqueTempDir("filetest");
    auto path = dir / "move.txt";
    {
        std::ofstream(path) << "ok";
    }

    FileID fid{path.string(), 5};
    File   f1(fid);
    ASSERT_EQ(f1.status(), File::FOK);
    auto ptr1 = f1.view().raw();
    auto sz1 = f1.view().size();

    File f2(std::move(f1));
    // Define behavior in your move ctor: f1 becomes empty
    EXPECT_TRUE(f2.view().raw() != nullptr);
    EXPECT_EQ(f2.view().size(), sz1);
}
