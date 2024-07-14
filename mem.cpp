#include "mem.h"
#include <memory>

void testFoo()
{
    std::cout << "The unique ptr 1" << std::endl;
    {
        auto sptr = std::make_shared<Foo>(100);
        std::cout << "Foo.bar: " << sptr->getBar() << ", count: " << sptr.use_count() << std::endl;
        std::cout << "before reset" << std::endl;
        sptr.reset();
        std::cout << "count: " << sptr.use_count() << std::endl;
    }

    std::cout << "The unique ptr 2" << std::endl;
    {
        auto sptr = std::make_shared<Foo>(200);
        std::cout << "Foo.bar: " << sptr->getBar() << ", count: " << sptr.use_count() << std::endl;
        std::cout << "before reset" << std::endl;
        sptr.reset(new Foo(222));
        std::cout << "count: " << sptr.use_count() << std::endl;
    }
}