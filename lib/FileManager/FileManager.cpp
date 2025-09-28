#include "scc/FileManager/FileManager.h"

using namespace scc;

File *FileManager::getFile(const std::string &Path) {
    FileID *FID = FF.getFileID(Path);
    if (!FID) {
        EM.report(err::error).msg("can't find ").msg(Path);
        return nullptr;
    }
    File F(*FID);
    if (F.status()) {
        auto [it, err] = Files.emplace(*FID, std::move(F));
        return &it->second;
    }
    EM.report(err::error).msg("faild to open ").msg(FID->getName());
    return nullptr;
}

File *FileManager::getSystemFile(const char *Path) {
    FileID *FID = FF.getSystemFileID(Path);
    if (!FID) {
        EM.report(err::error).msg("can't find ").msg(Path);
        return nullptr;
    }
    File F(*FID);
    if (F.status()) {
        auto [it, err] = Files.emplace(*FID, std::move(F));
        return &it->second;
    }
    EM.report(err::error).msg("faild to open ").msg(FID->getName());
    return nullptr;
}
