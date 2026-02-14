#include "KeyManager.h"

#include <gtest/gtest.h>
#include <filesystem>

using namespace std::filesystem;

TEST(KeyManagerHKDF, DeriveWorkKey_PurposeIsolation) {
    auto &km = KeyManager::Instance();
    km.Initialize("seed.bin");

    auto k1 = km.DeriveWorkKey("purpose-1", 32);
    auto k1b = km.DeriveWorkKey("purpose-1", 32);
    auto k2 = km.DeriveWorkKey("purpose-2", 32);

    ASSERT_EQ(k1.size(), 32);
    ASSERT_EQ(k1, k1b);               // same info -> same derived key
    ASSERT_NE(k1, k2);                // different info -> different derived key
}

TEST(KeyManagerHKDF, SaveReadEncryptedWorkKey_Roundtrip) {
    auto &km = KeyManager::Instance();
    km.Initialize("seed.bin");

    path tmp = temp_directory_path();
    path workFile = tmp / "km_test_work_key.bin";

    // ensure no preexisting file
    if (exists(workFile)) remove(workFile);

    auto wk1 = km.GetOrCreateWorkKey(workFile.string(), 32);
    ASSERT_EQ(wk1.size(), 32);
    ASSERT_TRUE(exists(workFile));

    // second call must read the same key from file
    auto wk2 = km.GetOrCreateWorkKey(workFile.string(), 32);
    ASSERT_EQ(wk2.size(), 32);
    ASSERT_EQ(wk1, wk2);

    // cleanup
    remove(workFile);
}
