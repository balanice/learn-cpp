#ifndef WIDGET_H
#define WIDGET_H

#include <spdlog/spdlog.h>

class Widget
{
private:
    int a;

public:
    Widget(int i, bool t);
    Widget(std::initializer_list<long double> il);
    ~Widget();
};

Widget::Widget(int i, bool t)
{
    spdlog::info("First init");
}

Widget::Widget(std::initializer_list<long double> il)
{
    spdlog::info("Second init");
}

Widget::~Widget()
{
}

#endif