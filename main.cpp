#include <cstring>
#include <format>
#include <future>
#include <iostream>

#include "spdlog/spdlog.h"

#include "curl_test.hpp"
#include "SingletonData.h"
#include "database.h"
#include "Crypt.h"
#include "MyCrypt2.h"
#include <thread>

void InitLog()
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug

    // change log pattern
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::debug("This message should be displayed..");
}

int SleepResult()
{
    spdlog::info("SleepResult");
    std::chrono::seconds s{3};
    std::this_thread::sleep_for(s);
    return 66;
}

int main(int, char**)
{
    // InitLog();

    // std::thread t1(InitLog);
    auto f = std::async(InitLog);
    // f.get();
    auto f1 = std::async(SleepResult);
    spdlog::info("f1: {}", f1.get());

    try {
        testCrypt();
    } catch (const std::exception &e) {
        spdlog::error("testCrypt error: {}", e.what());
    }

    try {
        testCrypt2();
    } catch (const std::exception &e) {
        spdlog::error("testCrypt2 error: {}", e.what());
    }

    std::string s = std::format("hello, {}", 99);
    spdlog::info("formated string: {}", s);

    int a{ 11 };
    spdlog::debug("value: {0}, poniter: {1}", a, fmt::ptr(&a));
    // Post();

    // testDb();

    // std::string tag{"hello"};
    // std::string value{"check"};
    // std::string value1{"check1"};
    // SingletonData::GetInstance().Put(tag, value);
    // SingletonData::GetInstance().Size(tag);
    // SingletonData::GetInstance().Put(tag, value);
    // SingletonData::GetInstance().Size(tag);

    /*MyDatabase db;
    db.CreateTable();
    if (db.Insert()) {
        std::cout << "Insert success" << std::endl;
    }
    db.Query();
    if (db.Delete(1)) {
        std::cout << "delete id 1 success" << std::endl;
    }
    std::cout << std::endl;
    db.Query();

    db.~MyDatabase();

    int* ar = new int[12];
    std::memset(ar, 99, sizeof ar);
    std::cout << "before for" << std::endl;
    for (int i = 0; i < 12; ++i) {
        ar[i] = 99;
        std::cout << ar[i] << std::endl;
    }
    delete[] ar;
    char* sr = new char[11];
    for (int i = 0; i < 10; ++i) {
        sr[i] = 19;
    }
    for (int i = 0; i < 11; ++i) {
        std::cout << sr[i] << std::endl;
    }
    delete[] sr;*/
}
