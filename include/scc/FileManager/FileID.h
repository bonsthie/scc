#ifndef SCC_FILEMANAGER_FILEID
#define SCC_FILEMANAGER_FILEID

#include <string>

namespace scc {

class FileID {
    std::string Name;
    int         ID;

  public:
    explicit FileID(const std::string &Name, const int ID) : Name(Name), ID(ID) {}
    explicit FileID(const FileID &FID) : Name(FID.Name), ID(FID.ID) {}

    const FileID &operator=(const FileID &FID);

    const std::string &getName() const { return Name; };
    const int          getFileFD() const { return ID; };

    bool operator>(const FileID &ID) const { return this->ID > ID.ID; }
    bool operator<(const FileID &ID) const { return this->ID < ID.ID; }
    bool operator>=(const FileID &ID) const { return this->ID >= ID.ID; }
    bool operator<=(const FileID &ID) const { return this->ID <= ID.ID; }
    bool operator==(const FileID &ID) const { return this->ID == ID.ID; }
    bool operator!=(const FileID &ID) const { return this->ID != ID.ID; }
};

}

#endif // SCC_FILEMANAGER_FILEID
