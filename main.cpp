#include <cstring>
#include <format>
#include <iostream>
#include "curl_test.hpp"
#include "SingletonData.h"
#include "database.h"
#include "Crypt.h"
#include "MyCrypt2.h"

int main(int, char**)
{
    try {
        testCrypt();
    } catch (const std::exception &e) {
        std::cout << "testCrypt error: " << e.what() << std::endl;
    }

    try {
        testCrypt2();
    } catch (const std::exception &e) {
        std::cout << "testCrypt2 error: " << e.what() << std::endl;
    }

    // std::string s = std::format("hello, {}", 99);
    // std::cout << s << std::endl;
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
