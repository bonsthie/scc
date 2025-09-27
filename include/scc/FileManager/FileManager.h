#ifndef SCC_FILEMANAGER_FILEMANAGER_H
#define SCC_FILEMANAGER_FILEMANAGER_H

#include <map>
#include <string>
#include "File.h"

namespace scc {

class FileManager {
    std::map<std::string, File> Files;
    // list of path where the FM can search

  public:
    FileManager() {}
    ~FileManager();

    // todo direct and undirect ("foo.h", <bar.h>)
    File &get(const char *path);
    File &get(const std::string &path);
};

} // namespace scc

#endif /* SCC_FILEMANAGER_FILEMANAGER_H */
