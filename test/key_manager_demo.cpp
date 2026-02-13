#include "KeyManager.h"

#include <spdlog/spdlog.h>
#include <sstream>
#include <iomanip>
#include <gtest/gtest.h>

TEST(KeyManagerTest, BasicAssertions)
{
    auto &km = KeyManager::Instance();
    km.Initialize("seed.bin", 32, 100000);
    auto hex = km.GetRootKeyHex();
    spdlog::info("Root key hex: {}", hex);

    auto work = km.GetOrCreateWorkKey("work_key.bin", 32);
    // print work key as hex
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (auto b : work)
        oss << std::setw(2) << (int)((unsigned)b);
    spdlog::info("Work key (hex): {}", oss.str());
}
