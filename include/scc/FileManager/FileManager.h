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

    File *getFile(const std::string &path);
    File *getSystemFile(const char *path);
};

} // namespace scc

#endif /* SCC_FILEMANAGER_FILEMANAGER_H */
