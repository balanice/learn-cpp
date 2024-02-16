#include <iostream>
#include <string>

#include <sqlite3.h>

#include "database.h"

static int callback(void *notUsed, int argc, char **argv, char **columnName)
{
    for (int i = 0; i < argc; ++i)
    {
        std::cout << columnName[i] << " = " << argv[i] << std::endl;
    }
    return 0;
}

void testDb()
{
    sqlite3 *db;
    int rc;
    // rc = sqlite3_open("test.db", &db);
    rc = sqlite3_open_v2("test.db", &db, SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE, NULL);
    if (rc)
    {
        std::cout << "open db failed: " << rc << std::endl;
        return;
    }
    std::cout << "open db success" << std::endl;
    std::string sql = "create table if not exists company(id PRIMARY KEY NOT NULL, name text, salary int);";
    char *errMsg;
    sqlite3_stmt *st;
    rc = sqlite3_prepare_v2(db, "", 0, &st, NULL);
    // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "Create table error" << rc << std::endl;
    }
    else
    {
        sqlite3_step(st);
        std::cout << "Create table success" << std::endl;
    }
    sqlite3_finalize(st);
    sqlite3_close(db);
}