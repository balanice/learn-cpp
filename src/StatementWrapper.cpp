#include "StatementWrapper.h"

#include <iostream>
#include <spdlog/spdlog.h>

StatementWrapper::StatementWrapper(const std::shared_ptr<sqlite3>& database, const std::string& query) :
    db(database)
{
    this->query = query;
    statement = prepareStmt();
}

std::shared_ptr<sqlite3_stmt> StatementWrapper::prepareStmt()
{
    sqlite3_stmt *stmt;
    const int res = sqlite3_prepare_v2(db.get(), query.c_str(), -1, &stmt, nullptr);
    if (res != SQLITE_OK) {
        auto msg = sqlite3_errmsg(db.get());
        spdlog::error("prepare failed: {}, errorCode: {}", msg, res);
        throw std::runtime_error("Failed to prepare statement");
    }
    return std::shared_ptr<sqlite3_stmt>(stmt, [](sqlite3_stmt *stmt) {
        sqlite3_finalize(stmt);
    });
}

sqlite3_stmt* StatementWrapper::getPreparedStatement() const
{
    return statement.get();
}

void StatementWrapper::Reset()
{
    sqlite3_reset(statement.get());
}

void StatementWrapper::ClearBindings()
{
    sqlite3_clear_bindings(statement.get());
}

void StatementWrapper::check(const int ret) const
{
    if (ret != SQLITE_OK) {
        auto msg = sqlite3_errmsg(db.get());
        spdlog::error("Check failed: {}", msg);
    }
}

int StatementWrapper::Step()
{
    return sqlite3_step(statement.get());
}

int StatementWrapper::Bind(int index, const int64_t value)
{
    int ret = sqlite3_bind_int64(statement.get(), index, value);
    check(ret);
    return ret;
}

int StatementWrapper::Bind(int index, const std::string& text)
{
    int ret = sqlite3_bind_text(statement.get(), index, text.c_str(), -1, SQLITE_TRANSIENT);
    check(ret);
    return ret;
}

int64_t StatementWrapper::ColumnInt(int index)
{
    return sqlite3_column_int64(statement.get(), index);
}

std::string StatementWrapper::ColumnText(int columnIndex)
{
    const unsigned char* text = sqlite3_column_text(statement.get(), columnIndex);
    if (!text) return std::string{};
    return std::string(reinterpret_cast<const char*>(text));
}