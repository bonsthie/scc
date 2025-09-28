
#include "scc/FileManager/FileID.h"

using namespace scc;

const FileID &FileID::operator=(const FileID &FID) {
    if (this == &FID)
        return *this;
    Name = FID.Name;
    ID = FID.ID;
    return *this;
}
