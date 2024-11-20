#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <memory>

class MyDatabase
{
private:
    std::shared_ptr<sqlite3> database;
    const int DB_VERSION{ 1 };
    const std::string TABLE_COMPANY{ "company" };
    const std::string TABLE_VERSION{ "db_version" };

public:
    MyDatabase();
    ~MyDatabase();
    bool CreateTable();
    bool Insert();
    bool Delete(int id);
    void Query();
    bool NeedUpgrade();

private:
    bool Exists(const std::string& tableName);
    bool Upgrade();
};

#endif