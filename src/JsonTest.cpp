#include "JsonTest.h"

#include <cjson/cJSON.h>
// #include <cjson/cJSON_Utils.h>
#include <spdlog/spdlog.h>

void TestJson()
{
    cJSON *a = cJSON_CreateObject();
    cJSON_AddFalseToObject(a, "isA");
    char *aStr = cJSON_Print(a);
    cJSON_Delete(a);
    spdlog::info("str: {}", aStr);

    std::string s = "{\"a\":\"cc\"";
    cJSON *sJ = cJSON_Parse(s.c_str());
    if (sJ == nullptr)
    {
        spdlog::warn("parse str error");
    }
    else
    {
        spdlog::info("parse success");
    }
    cJSON_Delete(sJ);
}
