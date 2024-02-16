#include <iostream>
#include "curl_test.hpp"
#include "SingletonData.h"
#include "database.h"

int main(int, char**) {
    std::cout << "Hello, from learn-cpp!\n";
    Post();

    testDb();

    std::string tag{"hello"};
    std::string value{"check"};
    std::string value1{"check1"};
    SingletonData::GetInstance().Put(tag, value);
    SingletonData::GetInstance().Size(tag);
    SingletonData::GetInstance().Put(tag, value);
    SingletonData::GetInstance().Size(tag);
}
