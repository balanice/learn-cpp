#ifndef RESOURCE_H
#define RESOURCE_H

#include <spdlog/spdlog.h>

class Resource
{
public:
    Resource()
    {
        spdlog::debug("Resource acquired");
    }

    ~Resource()
    {
        spdlog::debug("Resource destroyed");
    }
};
#endif