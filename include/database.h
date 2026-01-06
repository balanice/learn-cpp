#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <memory>
#include <vector>
#include "Message.h"

class MyDatabase
{
private:
    std::shared_ptr<sqlite3> database;
    const int DB_VERSION{ 1 };
    const std::string TABLE_VERSION{ "db_version" };
    const std::string TABLE_MESSAGES{ "messages" };

public:
    MyDatabase();
    ~MyDatabase();
    bool CreateTable();
    bool NeedUpgrade();

    // Message operations
    bool InsertMessage(const WriteMessage& msg);
    std::vector<WriteMessage> QueryMessages();

private:
    bool Exists(const std::string& tableName);
    bool Upgrade();
    bool Exec(const std::string& sql);
};

#endif