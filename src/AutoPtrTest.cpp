#include "AutoPtrTest.h"
#include "AutoPtr4.hpp"
#include "Resource.h"

AutoPtr4<Resource> GenerateResource()
{
    AutoPtr4<Resource> res{ new Resource };
    return res;
}

void TestAutoPtr()
{
    AutoPtr4<Resource> res1;
    res1 = GenerateResource();
}