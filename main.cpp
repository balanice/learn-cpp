#include <format>
#include <iostream>
#include "curl_test.hpp"
#include "SingletonData.h"
#include "database.h"

int main(int, char**)
{
    std::string s = std::format("hello, {}", 99);
    std::cout << s << std::endl;
    // Post();

    // testDb();

    // std::string tag{"hello"};
    // std::string value{"check"};
    // std::string value1{"check1"};
    // SingletonData::GetInstance().Put(tag, value);
    // SingletonData::GetInstance().Size(tag);
    // SingletonData::GetInstance().Put(tag, value);
    // SingletonData::GetInstance().Size(tag);

    MyDatabase db;
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
}
