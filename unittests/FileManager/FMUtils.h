#ifndef UNITEST_FILEMANAGER_UTILS_H
#define UNITEST_FILEMANAGER_UTILS_H

#include <filesystem>

std::filesystem::path MakeUniqueTempDir(const std::string &prefix);

void write_file(const std::filesystem::path &p, std::string_view content);

#endif // UNITEST_FILEMANAGER_UTILS_H
