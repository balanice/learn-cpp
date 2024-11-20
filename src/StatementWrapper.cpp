#include "StatementWrapper.h"

#include <iostream>
#include <spdlog/spdlog.h>

StatementWrapper::StatementWrapper(const std::shared_ptr<sqlite3> databse, const std::string& sql) :
    db(databse.get())
{
    query = sql;
    statement = prepareStmt();
}

std::shared_ptr<sqlite3_stmt> StatementWrapper::prepareStmt()
{
    sqlite3_stmt *stmt;
    const int res = sqlite3_prepare_v2(db, query.c_str(), static_cast<int>(query.size()), &stmt, nullptr);
    if (res != SQLITE_OK) {
        auto msg = sqlite3_errmsg(db);
        spdlog::error("prepare failed: {}, errorCode: {}", msg, res);
    }
    return std::shared_ptr<sqlite3_stmt>(stmt, [](sqlite3_stmt *stmt) {
        sqlite3_finalize(stmt);
    });
}

sqlite3_stmt* StatementWrapper::getPreparedStatement()
{
    return statement.get();
}

void StatementWrapper::check(const int ret) const
{
    if (ret != SQLITE_OK) {
        auto msg = sqlite3_errmsg(db);
        spdlog::error("Check failed: {}", msg);
    }
}

int StatementWrapper::Step()
{
    return sqlite3_step(statement.get());
}

void StatementWrapper::Bind(int index, const int64_t value)
{
    int ret = sqlite3_bind_int64(statement.get(), index, value);
    check(ret);
}

void StatementWrapper::Bind(int index, const std::string& text)
{
    int ret = sqlite3_bind_text(statement.get(), index, text.c_str(), static_cast<int>(text.size()), nullptr);
    check(ret);
}

int64_t StatementWrapper::ColumnInt(int index)
{
    return sqlite3_column_int64(statement.get(), index);
}

std::string StatementWrapper::ColumnText(int columnIndex)
{
    (void)sqlite3_column_bytes(statement.get(), columnIndex);
    auto data = static_cast<const char *>(sqlite3_column_blob(statement.get(), columnIndex));
    return std::string(data, sqlite3_column_bytes(statement.get(), columnIndex));
}