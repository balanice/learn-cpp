#include "database.h"

#include <format>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <sqlite3.h>

#include "StatementWrapper.h"

MyDatabase::MyDatabase()
{
    sqlite3 *db;
    int r = sqlite3_open_v2("test.db", &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
    if (r != SQLITE_OK)
    {
        std::cout << "open database failed: " << r << std::endl;
    }
    else
    {
        database = std::shared_ptr<sqlite3>(db, [](sqlite3 *db)
                                            { sqlite3_close(db); });
    }
}

MyDatabase::~MyDatabase()
{
    database.reset();
}

bool MyDatabase::CreateTable()
{
    if (Exists(TABLE_COMPANY))
    {
        spdlog::info("table already exists");
        return true;
    }
    std::string sql = "create table if not exists company(id integer PRIMARY KEY AUTOINCREMENT, name text, salary integer);";
    StatementWrapper wrapper(database, sql);
    return SQLITE_OK == wrapper.Step();
}

bool MyDatabase::Insert()
{
    std::string s = "INSERT INTO company(name, salary) VALUES (?, ?)";
    StatementWrapper wrapper(database, s);
    int index = 1;
    std::string name = "John";
    wrapper.Bind(index++, name);
    wrapper.Bind(index++, 22);
    return (wrapper.Step() == SQLITE_DONE);
}

bool MyDatabase::Delete(int id)
{
    std::string sql = "DELETE FROM company WHERE id=?;";
    StatementWrapper wrapper(database, sql);

    wrapper.Bind(1, id);
    int rc = wrapper.Step();
    if (rc != SQLITE_DONE)
    {
        auto msg = sqlite3_errmsg(database.get());
        std::cout << "execute delete failed: " << rc << ", msg: " << msg << std::endl;
        return false;
    }
    std::cout << "delete success" << std::endl;
    return true;
}

void MyDatabase::Query()
{
    std::string sql = "SELECT * FROM company;";
    StatementWrapper wrapper(database, sql);
    while (wrapper.Step() == SQLITE_ROW)
    {
        int index = 0;
        int id = wrapper.ColumnInt(index++);
        auto name = wrapper.ColumnText(index++);
        int salary = wrapper.ColumnInt(index++);
        spdlog::info("id: {}, name: {}, salary: {}", id, name, salary);
    }
}

bool MyDatabase::Exists(const std::string &tableName)
{
    const std::string sql = "select name from sqlite_master where type='table'";
    StatementWrapper st{database, sql};
    int valueIndex{0};
    while (st.Step() == SQLITE_ROW)
    {
        std::string v = st.ColumnText(valueIndex);
        spdlog::info("v: {}", v);
        if (v == tableName)
        {
            return true;
        }
    }
    return false;
}

bool MyDatabase::Upgrade()
{
    return false;
}

bool MyDatabase::NeedUpgrade()
{
    if (!Exists("db_version"))
    {
        spdlog::info("db_version not exists");
        return true;
    }
    const std::string sql = "select version from " + TABLE_VERSION;
    StatementWrapper st(database, sql);
    if (st.Step() == SQLITE_ROW)
    {
        auto v = st.ColumnInt(0);
        spdlog::info("db version: {}", v);
        return false;
    }
    return true;
}