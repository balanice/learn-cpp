#include "EffectiveC.h"

#include <spdlog/spdlog.h>

void TestConstexpr()
{
    constexpr int a{11};
    std::array<int, a> a1;

    int b{9};
    // std::array<int, b> b1;
}