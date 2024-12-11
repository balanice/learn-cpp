#include "Utils.h"

#include <spdlog/spdlog.h>

int main()
{
    auto count = GetCores();
    spdlog::info("cores: {}", count);
    if (count == 4) {
        return 0;
    }
    return 1;
}