#ifndef AUTO_PTR3_H
#define AUTO_PTR3_H
template<typename T>
class AutoPtr3
{
private:
    T* mPtr{};
public:
    AutoPtr3(T* ptr = nullptr)
        : mPtr { ptr }
    {
    }

    ~AutoPtr3()
    {
        delete mPtr;
    }

    AutoPtr3(const AutoPtr3& a)
    {
        mPtr = new T;
        *mPtr = *a.mPtr;
    }

    AutoPtr3& operator=(const AutoPtr3& a)
    {
        if (&a == this) {
            return *this;
        }
        delete mPtr;
        mPtr = new T;
        *mPtr = *a.mPtr;
        return *this;
    }

    T& operator*() const { return *mPtr; }
    T* operator->() const { return mPtr; }
    bool IsNull() const { return mPtr == nullptr; }
};

#endif