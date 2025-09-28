#include "scc/FileManager/File.h"

using namespace scc;

File::File(const FileID &FID) : FID(FID) {

    FD = ::open(FID.getName().c_str(), O_RDONLY);
    if (FD < 0) {
        FStatus = F_ERROR;
        return;
    }

    struct stat st;
    if (fstat(FD, &st) < 0) {
        FStatus = F_ERROR;
        return;
    }
    FileSize = st.st_size;

    Data = (char *)mmap(nullptr, FileSize, PROT_READ, MAP_PRIVATE, FD, 0);
    if (Data == MAP_FAILED) {
        FStatus = F_ERROR;
        return;
    }
}

File::~File() {
	if (Data && Data != MAP_FAILED)
        munmap(Data, FileSize);
    if (FD >= 0)
	::close(FD);
}
