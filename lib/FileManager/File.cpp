#include "scc/FileManager/File.h"

using namespace scc;

File::File(const FileID &FID) : FID(FID) {

    FD = ::open(FID.getName().c_str(), O_RDONLY);
    if (FD < 0) {
        FStatus = F_OPEN;
        return;
    }

    struct stat st;
    if (fstat(FD, &st) < 0) {
        FStatus = F_STAT;
        return;
    }
    FileSize = st.st_size;

    Data = (char *)mmap(nullptr, FileSize, PROT_READ, MAP_PRIVATE, FD, 0);
    if (Data == MAP_FAILED) {
        FStatus = F_MAPING;
        return;
    }
}

File::File(File &&F)
    : FID(std::move(F.FID)),
      FD(F.FD),
      Data(F.Data),
      FileSize(F.FileSize),
      FStatus(F.FStatus) {
    F.FD = -1;
    F.Data = nullptr;
    F.FileSize = 0;
    F.FStatus = F_ERROR;
}

File::~File() {
    if (Data && Data != MAP_FAILED)
        munmap(Data, FileSize);
    if (FD >= 0)
        ::close(FD);
}
