#include "scc/FileManager/FileManager.h"
#include "scc/Error/Error.h"

using namespace scc;

File *FileManager::getFile(const std::string &Name, const FileID &BaseFile) {
    FileID *FID = FF.getFileID(Name, BaseFile);
    return fileFormFileID(FID, Name);
}

File *FileManager::getFile(const std::string &Name, const File &BaseFile) {
    FileID *FID = FF.getFileID(Name, BaseFile.getFileID());
    return fileFormFileID(FID, Name);
}

File *FileManager::getFile(const std::string &Name, const std::string &BasePath) {
    FileID *FID = FF.getFileID(Name, BasePath);
    return fileFormFileID(FID, Name);
}

File *FileManager::getSystemFile(const std::string &Name) {
    FileID *FID = FF.getSystemFileID(Name);
    return fileFormFileID(FID, Name);
}

File *FileManager::fileFormFileID(const FileID *FID, const std::string &Path) {
    if (!FID) {
        if (Path.empty())
            EM.report(err::error).msg(" the file is a empty string");
        else
            EM.report(err::error).msg("can't find ").msg(Path);
        return nullptr;
    }

    // if the file already exist
    {
        auto it = Files.find(*FID);
        if (it != Files.end())
            return &it->second;
    }

    File F(*FID);
    if (F.status() == File::FOK) {
        auto [it, err] = Files.emplace(*FID, std::move(F));
        if (err)
            return &it->second;
    }

    std::string FileName(Path.empty() ? FID->getName() : Path);
    EM.report(err::error).msg("faild to open ").msg(FileName);
    return nullptr;
}
