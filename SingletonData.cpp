#include <iostream>
#include "include/SingletonData.h"

bool SingletonData::Registered(std::string &tag)
{
    return data.find(tag) != data.end();
}

void SingletonData::Put(std::string &tag, std::string &value)
{
    std::shared_ptr<std::vector<std::string>> v;
    if (Registered(tag)) {
        v = data[tag];
    } else {
        v = std::make_shared<std::vector<std::string>>();
        data.insert({tag, v});
    }
    v.get()->push_back(value);
}

void SingletonData::Size(std::string &tag)
{
    if (!Registered(tag)) {
        std::cout << "size is 0" << std::endl;
        return;
    }
    auto s = data[tag]->size();
    std::cout << "size: " << s << std::endl;
}