#include "Config.h"

#include <gtest/gtest.h>
#include <filesystem>

TEST(ConfigTest, SaveLoadRoundTrip) {
    namespace fs = std::filesystem;
    auto tmp = fs::temp_directory_path();
    auto cfgFile = tmp / "test_config_gtest.cfg";
    auto dataDir = tmp / "data_test_gtest";

    Config c1;
    c1.setStoragePath(dataDir.string());
    c1.setEncrypted(true);
    ASSERT_TRUE(c1.saveToFile(cfgFile.string()));

    Config c2;
    ASSERT_TRUE(c2.loadFromFile(cfgFile.string()));

    EXPECT_EQ(c2.getStoragePath(), dataDir.string());
    EXPECT_TRUE(c2.isEncrypted());

    std::error_code ec;
    fs::remove(cfgFile, ec);
}
