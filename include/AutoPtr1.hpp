#ifndef AUTO_PTR1_HPP
#define AUTP_PTR1_HPP

#include <spdlog/spdlog.h>

template <typename T>
class AutoPtr1
{
    T* mPtr{};
public:
    AutoPtr1(T* ptr=nullptr)
        : mPtr(ptr)
    {
    }

    ~AutoPtr1()
    {
        delete mPtr;
    }

    T& operator*() const { return *mPtr; }
    T* operator->() const { return mPtr; }
};

#endif