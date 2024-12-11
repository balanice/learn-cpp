#include "Utils.h"

#include <sys/stat.h>
#include <thread>

bool Exists(const std::string &file)
{
    struct stat st;
    return stat(file.c_str(), &st) == 0;
}

unsigned int GetCores()
{
    return std::thread::hardware_concurrency();
}