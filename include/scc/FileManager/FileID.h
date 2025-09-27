#ifndef SCC_FILEMANAGER_FILEID
#define SCC_FILEMANAGER_FILEID

#include <string>

class FileID {
    const std::string Name;
    const int         FileFD;

  public:
    explicit FileID(const std::string Name, const int FileFD) : Name(Name), FileFD(FileFD) {}

    const std::string &getName() const { return Name; };
    const int          getFileFD() const { return FileFD; };
};

#endif // SCC_FILEMANAGER_FILEID
