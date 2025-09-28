#ifndef SCC_FILEMANAGER_FILEMANAGER_H
#define SCC_FILEMANAGER_FILEMANAGER_H

#include "File.h"
#include "FileFinder.h"
#include "FileID.h"
#include "scc/Error/ErrorManager.h"
#include <map>
#include <string>

namespace scc {

class FileManager {
    std::map<FileID, File> Files;

    FileFinder   &FF;
    ErrorManager &EM;

  public:
    FileManager(FileFinder &FF, ErrorManager &EM) : FF(FF), EM(EM) {}
    ~FileManager() = default;

    /// return the File for the Name file from the BaseFileID or the System path
    File *getFile(const std::string &Name, const FileID &BaseFile);
    /// return the File for the Name file from the BaseFile or the System path
    File *getFile(const std::string &Name, const File &BaseFile);
    /// return the File for the Name file from the BasePath or the System path
    File *getFile(const std::string &Name, const std::string &BasePath = "");
    /// return a File for Name base On the System Path
    File *getSystemFile(const std::string &Name);

    File *fileFormFileID(const FileID *FID, const std::string &Path = "");
};

} // namespace scc

#endif /* SCC_FILEMANAGER_FILEMANAGER_H */
