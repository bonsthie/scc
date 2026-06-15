#include "scc/FileManager/FileFinder.h"
#include <filesystem>

using namespace scc;

FileFinder::FileFinder(const scc::Vector<std::string> &SysPaths) { addSystemPaths(SysPaths); };

FileID *FileFinder::getFileID(const std::string &Name, const FileID &BaseFile) {
    if (Name.empty())
        return nullptr;
    std::filesystem::path Np(Name);
    if (Np.is_absolute())
        return getFileID(Name);

    std::filesystem::path BasePath(BaseFile.getName());
    return getFileID(Name, BasePath.parent_path().string() + "/");
}

FileID *FileFinder::getFileID(const std::string &Name, const std::string &BasePath) {
    if (Name.empty())
        return nullptr;
    const std::string AbsPathFile = absolutePath(BasePath + Name);

    if (AbsPathFile.empty())
        return getSystemFileID(Name);

    auto FID = AbsPathFiles.find(AbsPathFile);
    if (FID != AbsPathFiles.end())
        return &FID->second;

    return openNewRelativeFileFromAbsPath(AbsPathFile);
}

FileID *FileFinder::getSystemFileID(const std::string &Name) {
    if (Name.empty())
        return nullptr;

    auto FID = SystemPathFiles.find(Name);
    if (FID == SystemPathFiles.end())
        return openNewSystemFile(Name);
    return &FID->second;
}

void FileFinder::addSystemPaths(const scc::Vector<std::string> &SysPaths) {
    for (auto Path : SysPaths) {
        addSystemPath(Path);
    }
}

void FileFinder::addSystemPath(const std::string &SysPath) {
    std::error_code Ec;

    auto Abs = std::filesystem::absolute(SysPath, Ec);
    if (Ec)
        return;

    auto c = std::filesystem::weakly_canonical(Abs, Ec);
    if (Ec)
        return;
    SystemPaths.pushBack(c);
}

FileID *FileFinder::openNewSystemFile(const std::string &Name) {
    for (const auto &Dir : SystemPaths) {
        std::error_code Ec;

        std::filesystem::path Candidate = Dir / Name;

        auto Canon = std::filesystem::weakly_canonical(Candidate, Ec);
        if (!std::filesystem::exists(Canon, Ec) || Ec)
            continue;

        std::string CanonStr = Canon.string();

        FileID *FID = nullptr;
        auto    It = AbsPathFiles.find(CanonStr);

        if (It == AbsPathFiles.end())
            FID = openNewRelativeFileFromAbsPath(CanonStr);
        else
            FID = &It->second;

        auto ItSys = SystemPathFiles.find(Name);
        if (ItSys == SystemPathFiles.end())
            SystemPathFiles.emplace(Name, *FID);
        return FID;
    }
    return nullptr;
}

FileID *FileFinder::openNewRelativeFileFromAbsPath(const std::string &Name) {
    FileID Fid{Name, ID++};
    auto [it, inserted] = AbsPathFiles.emplace(Name, std::move(Fid));
    return &it->second;
}

std::string FileFinder::absolutePath(const std::string &Path) {
    std::error_code Ec;

    auto Abs = std::filesystem::absolute(Path, Ec);
    if (Ec)
        return "";

    auto c = std::filesystem::weakly_canonical(Abs, Ec);
    if (Ec)
        return "";

    if (!std::filesystem::exists(Abs))
        return "";

    return c.string();
}
