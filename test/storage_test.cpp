#include "Storage.h"
#include "Config.h"
#include "database.h"

#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>
#include <filesystem>

void removeDbAndFiles(const std::string& storagePath) {
    // remove sqlite file used by MyDatabase (test.db in working directory)
    try { std::filesystem::remove("test.db"); } catch(...) {}
    try { std::filesystem::remove_all(storagePath); } catch(...) {}
}

int main() {
    spdlog::set_level(spdlog::level::info);

    // Clean up from previous runs
    removeDbAndFiles("test_storage_plain");

    // Test 1: plain storage
    Config c1;
    c1.setStoragePath("test_storage_plain");
    c1.setEncrypted(false);
    {
        Storage s(c1);
        s.store("plain_key", "plain_value");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // trigger report to show stored values
        s.report();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        s.stop(true);
    }

    MyDatabase db;
    auto msgs = db.QueryMessages();
    bool foundPlain = false;
    for (auto &m : msgs) {
        if (m.key == "plain_key" && m.value == "plain_value") { foundPlain = true; }
    }
    if (!foundPlain) {
        spdlog::error("Plain storage test failed: entry not found");
        return 1;
    }

    // Test 2: encrypted storage
    removeDbAndFiles("test_storage_enc");
    Config c2;
    c2.setStoragePath("test_storage_enc");
    c2.setEncrypted(true);
    {
        Storage s(c2);
        s.store("enc_key", "secret_value");
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        // trigger report -- MessageWorker should decrypt and display
        s.report();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        s.stop(true);
    }

    MyDatabase db2;
    auto msgs2 = db2.QueryMessages();
    bool foundEnc = false;
    for (auto &m : msgs2) {
        if (m.key == "enc_key") {
            if (m.value.rfind("ENC:", 0) == 0) foundEnc = true;
        }
    }
    if (!foundEnc) {
        spdlog::error("Encrypted storage test failed: encrypted entry not found");
        return 1;
    }

    // Clean up
    try { std::filesystem::remove("test.db"); } catch(...) {}
    try { std::filesystem::remove_all("test_storage_plain"); } catch(...) {}
    try { std::filesystem::remove_all("test_storage_enc"); } catch(...) {}

    spdlog::info("Storage tests passed");
    return 0;
}
