#include "scc/FileManager/FileFinder.h"

using namespace scc;

FileFinder::FileFinder(const std::vector<std::string> &SysPaths) { addSystemPaths(SysPaths); };

FileID *FileFinder::getFileID(const std::string &Name, const FileID &BaseFile) {
    if (Name.empty())
        return nullptr;
    std::filesystem::path np(Name);
    if (np.is_absolute())
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

/// return a FileID for Name base On the System Path
FileID *FileFinder::getSystemFileID(const std::string &Name) {
    auto FID = SystemPathFiles.find(Name);
    if (FID == SystemPathFiles.end())
        return openNewSystemFile(Name);
    return &FID->second;
}

void FileFinder::addSystemPaths(const std::vector<std::string> &SysPaths) {
    for (auto Path : SysPaths) {
		addSystemPath(Path);

    }
}

void FileFinder::addSystemPath(const std::string &SysPath) {
        std::error_code ec;

        auto abs = std::filesystem::absolute(SysPath, ec);
        if (ec)
            return;

        auto c = std::filesystem::weakly_canonical(abs, ec);
        if (ec)
            return;
        SystemPaths.push_back(c);
}

FileID *FileFinder::openNewSystemFile(const std::string &Name) {
    for (const auto &dir : SystemPaths) {
        std::error_code ec;

        std::filesystem::path Candidate = dir / Name;

        auto Canon = std::filesystem::weakly_canonical(Candidate, ec);
        if (!std::filesystem::exists(Canon, ec) || ec)
            continue;

        std::string CanonStr = Canon.string();

        FileID *FID = nullptr;
        auto    it = AbsPathFiles.find(CanonStr);

        if (it == AbsPathFiles.end())
            FID = openNewRelativeFileFromAbsPath(CanonStr);
        else
            FID = &it->second;

        auto itSys = SystemPathFiles.find(Name);
        if (itSys == SystemPathFiles.end())
            SystemPathFiles.emplace(Name, *FID);
        return FID;
    }
    return nullptr;
}

FileID *FileFinder::openNewRelativeFileFromAbsPath(const std::string &Name) {
    FileID fid{Name, ID++};
    auto [it, inserted] = AbsPathFiles.emplace(Name, std::move(fid));
    return &it->second;
}

std::string FileFinder::absolutePath(const std::string &path) {
    std::error_code ec;

    auto abs = std::filesystem::absolute(path, ec);
    if (ec)
        return "";

    auto c = std::filesystem::weakly_canonical(abs, ec);
    if (ec)
        return "";

    return c.string();
}
