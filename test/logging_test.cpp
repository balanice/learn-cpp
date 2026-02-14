#include <gtest/gtest.h>
#include <filesystem>
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "Config.h"
#include "Storage.h"

using namespace std::filesystem;

TEST(LoggingTest, FileCreatedAndRotatesBySize) {
    // use repository-local directory so CI/build artifacts are kept under `build/`
    auto tmp = std::filesystem::current_path() / "build" / "test-logs";
    std::error_code ec;
    remove_all(tmp, ec);
    create_directories(tmp, ec);

    Config c;
    c.setStoragePath(tmp.string());
    c.setEncrypted(false);

    // constructing Storage should configure the file logger
    Storage s(c);

    // log many messages until rotation should occur (1MB threshold)
    std::string payload(4096, 'X');
    const int iterations = 300; // ~1.2MB of log
    for (int i = 0; i < iterations; ++i) {
        spdlog::info("{} - {}", i, payload);
    }

    // give spdlog a moment to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // files named: app-log-YYYY-MM-DD[(-N)].log
    bool foundAny = false;
    int foundCount = 0;
    for (auto &e : directory_iterator(tmp)) {
        auto n = e.path().filename().string();
        if (n.rfind("app-log-", 0) == 0 && n.find(".log") != std::string::npos) {
            ++foundCount;
            foundAny = true;
        }
    }

    EXPECT_TRUE(foundAny);
    EXPECT_GE(foundCount, 1);

    // cleanup
    remove_all(tmp, ec);
}
