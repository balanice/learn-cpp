#include "MessageWorker.h"
#include "Message.h"
#include "spdlog/spdlog.h"
#include "MyCrypt2.h"
#include "Config.h"
#include "Storage.h"

#include <filesystem>

void InitLog()
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    // change log pattern
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
}

int main(int, char **)
{
    InitLog();

    spdlog::info("=== Demo: plain and encrypted storage with report ===");

    // Ensure clean DB for demo
    try { std::filesystem::remove("test.db"); } catch(...) {}

    // Plain storage demo
    spdlog::info("-- Plain storage demo --");
    Config cfgPlain;
    cfgPlain.setStoragePath("demo_plain");
    cfgPlain.setEncrypted(false);
    {
        Storage storage(cfgPlain);
        storage.store("plain_key", "plain_value");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        storage.report(); // should show plain_value
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        storage.stop(true);
    }

    // Encrypted storage demo
    spdlog::info("-- Encrypted storage demo --");
    Config cfgEnc;
    cfgEnc.setStoragePath("demo_enc");
    cfgEnc.setEncrypted(true);
    {
        Storage storage(cfgEnc);
        storage.store("secret_key", "super_secret_value");
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        storage.report(); // MessageWorker should decrypt and display value
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        storage.stop(true);
    }

    spdlog::info("Demo finished.");

    // Optional: run crypt tests
    testCrypt2();

    return 0;
}
