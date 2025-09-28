#include <filesystem>
#include <fstream>
#include <random>

namespace fs = std::filesystem;

fs::path MakeUniqueTempDir(const std::string &prefix) {
    const fs::path base = fs::temp_directory_path();

    for (int i = 0; i < 64; ++i) {
        auto            now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937_64 rng(static_cast<uint64_t>(now) ^ (static_cast<uint64_t>(i) << 32));
        uint64_t        r = rng();

        fs::path candidate = base / (prefix + "-" + std::to_string(now) + "-" + std::to_string(r));
        std::error_code ec;
        if (fs::create_directories(candidate, ec)) {
            return candidate;
        }
    }

    throw std::runtime_error("MakeUniqueTempDir: failed to create a unique directory");
}


void write_file(const fs::path &p, std::string_view content) {
    std::ofstream ofs(p);
    ofs << content;
}

