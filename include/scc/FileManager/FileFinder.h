#ifndef SCC_FILEMANGER_FILEFINDER
#define SCC_FILEMANGER_FILEFINDER

#include <atomic>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "FileID.h"

namespace scc {

class FileFinder {

    // use for system path include <foo.h> or -internal-isystem
    std::vector<std::filesystem::path>        SystemPaths;
    std::unordered_map<std::string, FileID &> SystemPathFiles;

    std::unordered_map<std::string, FileID> AbsPathFiles;

    std::atomic_int ID = 0;

  public:
    FileFinder(const std::vector<std::string> &SysPaths);

    /// return the FileID for the Name file from the BaseFile or the System path
    FileID *getFileID(const std::string &Name, const FileID &BaseFile);
    /// return the FileID for the Name file from the BasePath or the System path
    FileID *getFileID(const std::string &Name, const std::string &BasePath = "");
    /// return a FileID for Name base On the System Path
    FileID *getSystemFileID(const std::string &Name);

    void addSystemPath(const std::string &SysPath);
    void addSystemPaths(const std::vector<std::string> &SysPaths);

  private:
    FileID *openNewSystemFile(const std::string &Name);
    // create a FileID for a new File (Name) base on relative path
    FileID *openNewRelativeFileFromAbsPath(const std::string &Name);

    std::string absolutePath(const std::string &path);
};

} // namespace scc

#endif // SCC_FILEMANGER_FILEFINDER
