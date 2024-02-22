#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>

void testDb();

class MyDatabase
{
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;

public:
    MyDatabase();
    ~MyDatabase();
    bool CreateTable();
    bool Insert();
    bool Delete(int id);
    void Query();
};

#endif