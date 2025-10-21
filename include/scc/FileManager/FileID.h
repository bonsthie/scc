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

    std::string getBaseName() {
        size_t pos = Name.find_last_of("/\\");
        return (pos == std::string::npos) ? Name : Name.substr(pos + 1);
    }

    bool operator>(const FileID &ID) const { return this->ID > ID.ID; }
    bool operator<(const FileID &ID) const { return this->ID < ID.ID; }
    bool operator>=(const FileID &ID) const { return this->ID >= ID.ID; }
    bool operator<=(const FileID &ID) const { return this->ID <= ID.ID; }
    bool operator==(const FileID &ID) const { return this->ID == ID.ID; }
    bool operator!=(const FileID &ID) const { return this->ID != ID.ID; }
};

} // namespace scc

#endif // SCC_FILEMANAGER_FILEID
