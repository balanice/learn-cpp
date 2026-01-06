#include "database.h"

#include <format>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <sqlite3.h>

#include "StatementWrapper.h"
#include "Message.h"
#include <vector>

MyDatabase::MyDatabase()
{
    sqlite3 *db;
    int r = sqlite3_open_v2("test.db", &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
    if (r != SQLITE_OK)
    {
        spdlog::error("open database failed: {}", r);
        throw std::runtime_error("Failed to open database");
    }
    database = std::shared_ptr<sqlite3>(db, [](sqlite3 *db)
                                        { sqlite3_close(db); });
}

MyDatabase::~MyDatabase()
{
    database.reset();
}

bool MyDatabase::CreateTable()
{
    // Ensure messages table exists
    std::string sql2 = "create table if not exists messages(id integer PRIMARY KEY AUTOINCREMENT, key text, value text);";
    StatementWrapper wrapper2(database, sql2);
    bool okMessages = (wrapper2.Step() == SQLITE_DONE);

    if (!okMessages)
    {
        spdlog::error("failed to create or verify messages table");
    }

    return okMessages;
}

bool MyDatabase::InsertMessage(const WriteMessage &msg)
{
    std::string s = "INSERT INTO messages(key, value) VALUES (?, ?)";
    StatementWrapper wrapper(database, s);
    int index = 1;
    wrapper.Bind(index++, msg.key);
    wrapper.Bind(index++, msg.value);
    return (wrapper.Step() == SQLITE_DONE);
}

std::vector<WriteMessage> MyDatabase::QueryMessages()
{
    std::vector<WriteMessage> messages;
    std::string sql = "SELECT key, value FROM messages;";
    StatementWrapper wrapper(database, sql);
    while (wrapper.Step() == SQLITE_ROW)
    {
        WriteMessage m;
        m.key = wrapper.ColumnText(0);
        m.value = wrapper.ColumnText(1);
        messages.push_back(std::move(m));
    }
    return messages;
}

bool MyDatabase::Exists(const std::string& tableName) {
    const std::string sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?;";
    StatementWrapper st(database, sql);
    st.Bind(1, tableName);
    if (st.Step() == SQLITE_ROW) {
        return st.ColumnInt(0) > 0;
    }
    return false;
}

bool MyDatabase::Exec(const std::string& sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(database.get(), sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        spdlog::error("Exec failed: {}, sql: {}", errmsg ? errmsg : "unknown", sql);
        sqlite3_free(errmsg);
        return false;
    }
    return true;
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