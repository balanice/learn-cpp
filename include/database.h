#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <memory>

class MyDatabase
{
private:
    std::shared_ptr<sqlite3> database;

public:
    MyDatabase();
    ~MyDatabase();
    bool CreateTable();
    bool Insert();
    bool Delete(int id);
    void Query();
};

#endif