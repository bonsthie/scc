#ifndef SCC_FILEMANAGER_FILEID_H
#define SCC_FILEMANAGER_FILEID_H

#include <string>

namespace scc {

class FileID {
    std::string Name;
    int         ID;

  public:
    explicit FileID(const std::string &Name, const int ID) : Name(Name), ID(ID) {}
    explicit FileID(const FileID &FID) : Name(FID.Name), ID(FID.ID) {}

    FileID &operator=(const FileID &FID);

    const std::string &getName() const { return Name; };
    int                getFileFD() const { return ID; };

    std::string getBaseName() const {
        size_t Pos = Name.find_last_of("/\\");
        return (Pos == std::string::npos) ? Name : Name.substr(Pos + 1);
    }

    bool operator>(const FileID &ID) const { return this->ID > ID.ID; }
    bool operator<(const FileID &ID) const { return this->ID < ID.ID; }
    bool operator>=(const FileID &ID) const { return this->ID >= ID.ID; }
    bool operator<=(const FileID &ID) const { return this->ID <= ID.ID; }
    bool operator==(const FileID &ID) const { return this->ID == ID.ID; }
    bool operator!=(const FileID &ID) const { return this->ID != ID.ID; }
};

} // namespace scc

#endif // SCC_FILEMANAGER_FILEID_H
