#ifndef SCC_FILE_H
#define SCC_FILE_H

#include "scc/FileManager/FileID.h"
#include "scc/FileManager/MemoryBufferView.h"
#include <string>

namespace scc {

class File {
    FileID FID;
    int    FD = -1;
    char  *Data = nullptr;
    size_t FileSize = 0;

  public:
    // mmap the file wr, setup the size and setup the FileStatus
    File(const FileID &FID);
    File(File &&F);

    // unmap the file
    ~File();

    MemoryBufferView view() const { return MemoryBufferView(Data, FileSize); };
    size_t           size() const { return FileSize; };
    const FileID    &getFileID() const { return FID; }

    enum FileStatus { FOK, F_ERROR, F_OPEN, F_STAT, F_MAPING };

  private:
    FileStatus FStatus = FOK;

  public:
    FileStatus status() const { return FStatus; };
};

} // namespace scc

#endif // SCC_FILE_H
