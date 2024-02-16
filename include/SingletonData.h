#ifndef SINGLETON_DATA_H
#define SINGLETON_DATA_H

#include <map>
#include <memory>
#include <vector>

class SingletonData
{
private:
    std::map<std::string, std::shared_ptr<std::vector<std::string>>> data;
    SingletonData() = default;
public:
    ~SingletonData() = default;
    inline bool Registered(std::string &tag);
    void Put(std::string &tag, std::string &value);
    void Size(std::string &tag);

    static SingletonData& GetInstance() {
        static SingletonData s;
        return s;
    }
};

#endif