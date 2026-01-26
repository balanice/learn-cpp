#include "Config.h"
#include "spdlog/spdlog.h"
#include <filesystem>

int main() {
    spdlog::set_level(spdlog::level::info);

    const std::string fname = "test_config.cfg";

    Config c1;
    c1.setStoragePath("/tmp/data_test");
    c1.setEncrypted(true);
    if (!c1.saveToFile(fname)) {
        spdlog::error("Failed to save config");
        return 1;
    }

    Config c2;
    if (!c2.loadFromFile(fname)) {
        spdlog::error("Failed to load config");
        return 1;
    }

    if (c2.getStoragePath() != "/tmp/data_test") {
        spdlog::error("storage_path mismatch: {}", c2.getStoragePath());
        return 1;
    }
    if (!c2.isEncrypted()) {
        spdlog::error("encrypt flag mismatch");
        return 1;
    }

    spdlog::info("Config test passed: storage_path={}, encrypt={}", c2.getStoragePath(), c2.isEncrypted());

    std::filesystem::remove(fname);
    return 0;
}
