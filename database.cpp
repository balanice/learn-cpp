#include <format>
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

MyDatabase::MyDatabase()
{
    int r = sqlite3_open_v2("test.db", &db, SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE, NULL);
    if (r != SQLITE_OK) {
        std::cout << "open database failed: " << r << std::endl;
        return;
    }

}

MyDatabase::~MyDatabase()
{
    if (db) {
        sqlite3_close(db);
    }
}

bool MyDatabase::CreateTable()
{
    std::string s = std::format("hello, from {}", 99);
    std::cout << "formatted: " << s << std::endl;
    std::string sql = "create table if not exists company(id integer PRIMARY KEY AUTOINCREMENT, name text, salary integer);";
    char *errMsg;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    // rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cout << "prepare error: " << rc << std::endl;
        return false;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cout << "Create table failed:" << rc << std::endl;
        return false;
    }
    sqlite3_reset(stmt);
    return true;
}

bool MyDatabase::Insert()
{
    std::string s = "INSERT INTO company(name, salary) VALUES (?, ?)";
    int rc = sqlite3_prepare_v2(db, s.c_str(), s.length(), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cout << "prepare insert failed: " << rc << std::endl;
        return false;
    }
    std::string name = "John";
    rc = sqlite3_bind_text(stmt, 1, name.c_str(), name.length(), NULL);
    if (rc != SQLITE_OK)
    {
        std::cout << "bind text failed: " << rc << std::endl;
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, 21);
    if (rc != SQLITE_OK)
    {
        std::cout << "bind int failed: " << rc << std::endl;
        return false;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cout << "step failed: " << rc << std::endl;
        return false;
    }
    
    sqlite3_reset(stmt);
    return true;
}

bool MyDatabase::Delete(int id)
{
    std::string sql = "DELETE FROM company WHERE id=?;";
    int rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
    if (rc != SQLITE_OK) {
        const char *msg = sqlite3_errmsg(db);
        std::cout << "prepare delete failed: " << rc << ", msg: " << msg << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        auto msg = sqlite3_errmsg(db);
        std::cout << "execute delete failed: " << rc << ", msg: " << msg << std::endl;
        return false;
    }
    std::cout << "delete success" << std::endl;
    sqlite3_reset(stmt);
    return true;
}

void MyDatabase::Query()
{
    std::string sql = "SELECT * FROM company";
    int rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "prepare query failed: " << rc << std::endl;
        return;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        auto name = sqlite3_column_text(stmt, 1);
        int salary = sqlite3_column_int(stmt, 2);
        std::cout << "id: " <<id << ", name: " << name << ", salary: " << salary << std::endl;
    }
    sqlite3_reset(stmt);
}