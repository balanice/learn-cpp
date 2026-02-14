#include "Storage.h"
#include "Config.h"
#include "database.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <thread>

using namespace std::filesystem;

struct StorageTest : public ::testing::Test {
    path tmp = temp_directory_path();
    path plainDir = tmp / "test_storage_plain_gtest";
    path encDir = tmp / "test_storage_enc_gtest";

    void SetUp() override {
        cleanup();
    }
    void TearDown() override {
        cleanup();
    }
    void cleanup() {
        // remove sqlite file used by MyDatabase (test.db in working directory)
        std::error_code ec;
        std::filesystem::remove("test.db", ec);
        std::filesystem::remove_all(plainDir, ec);
        std::filesystem::remove_all(encDir, ec);
    }
};

TEST_F(StorageTest, PlainStorageWritesToDb) {
    Config c;
    c.setStoragePath(plainDir.string());
    c.setEncrypted(false);

    {
        Storage s(c);
        s.store("plain_key", "plain_value");
        // ensure worker drains
        s.stop(true);
    }

    MyDatabase db;
    auto msgs = db.QueryMessages();

    bool found = false;
    for (const auto &m : msgs) {
        if (m.key == "plain_key" && m.value == "plain_value") { found = true; break; }
    }
    EXPECT_TRUE(found);
}

TEST_F(StorageTest, EncryptedStorageStoresEncryptedValue) {
    Config c;
    c.setStoragePath(encDir.string());
    c.setEncrypted(true);

    {
        Storage s(c);
        s.store("enc_key", "secret_value");
        s.stop(true);
    }

    MyDatabase db;
    auto msgs = db.QueryMessages();

    bool found = false;
    for (const auto &m : msgs) {
        if (m.key == "enc_key") {
            if (m.value.rfind("ENC:", 0) == 0) found = true;
        }
    }
    EXPECT_TRUE(found);
}
