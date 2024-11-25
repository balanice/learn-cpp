#include "Utils.h"

#include <sys/stat.h>

bool Exists(const std::string &file)
{
    struct stat st;
    return stat(file.c_str(), &st) == 0;
}