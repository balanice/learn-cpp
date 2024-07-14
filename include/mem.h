#ifndef FORCE_MEM_H
#define FORCE_MEM_H

#include <iostream>

struct Foo
{
    Foo(int n = 0) : bar(n)
    {
        std::cout << bar << "@" << this << std::endl;
    }
    ~Foo()
    {
        std::cout << "~Foo" << this << std::endl;
    }
    int getBar() const noexcept
    {
        return bar;
    }
    private:
        int bar;
};

void testFoo();
#endif