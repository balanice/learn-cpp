#ifndef AUTO_PTR4_HPP
#define AUTO_PTR4_HPP
template <typename T>
class AutoPtr4
{
private:
    T* mPtr;
public:
    AutoPtr4(T* ptr = nullptr)
        : mPtr(ptr)
    {
    }

    ~AutoPtr4()
    {
        delete mPtr;
    }

    // copy constructor
    AutoPtr4(const AutoPtr4& a)
    {
        mPtr = new T;
        *mPtr = *a.mPtr;
    }

    // move constructor
    AutoPtr4(AutoPtr4&& a) noexcept
        : mPtr(a.mPtr)
    {
        a.mPtr = nullptr;
    }

    // copy assignment
    AutoPtr4& operator=(const AutoPtr4& a)
    {
        if (&a == this)
            return *this;
        
        delete mPtr;
        mPtr = new T;
        *mPtr = *a.mPtr;
    }

    // move assignment
    AutoPtr4& operator=(AutoPtr4&& a) noexcept
    {
        if (&a == this)
            return *this;
        
        delete mPtr;
        mPtr = a.mPtr;
        a.mPtr = nullptr;

        return *this;
    }

    T& operator*() { return *mPtr; }
    T* operator->() { return mPtr; }
    bool IsNull() const { return mPtr == nullptr; }
};

#endif