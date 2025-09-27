#ifndef SCC_FILEMANAGER_FILE
#define SCC_FILEMANAGER_FILE

#include "scc/FileManager/MemoryBufferView.h"
#include <string>

namespace scc {

class File {
    const std::string &FileName;
    const int          id = 0;
    size_t             FileSize = 0;
    const char        *Data = nullptr;

  public:
    // mmap the file wr, setup the size and setup the FileStatus
    File(const std::string &FileName, const int id);

    // unmap the file
    ~File();

    const MemoryBufferView view() const { return MemoryBufferView(Data, FileSize); };

    enum FileStatus {
        FOK,
        F_ERROR, // todo : more advence error
    };

  private:
    FileStatus FileStatus = FOK;
};

} // namespace scc

#endif // SCC_FILEMANAGER_FILE
