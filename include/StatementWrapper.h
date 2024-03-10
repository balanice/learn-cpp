#ifndef STATEMENT_WRAPPER_H
#define STATEMENT_WRAPPER_H

#include <memory>
#include <string>
#include <sqlite3.h>

class StatementWrapper
{
private:
    sqlite3 *db;
    std::shared_ptr<sqlite3_stmt> statement;
    std::string query;

    std::shared_ptr<sqlite3_stmt> prepareStmt();
    void check(const int ret) const;
public:
    StatementWrapper(const std::shared_ptr<sqlite3> databse, const std::string& query);
    ~StatementWrapper() = default;
    sqlite3_stmt* getPreparedStatement();
    int Step();

    /**
     * Bind text to index(>=0).
     */
    void Bind(int bindIndex, const std::string& text);
    void Bind(int bindIndex, const int64_t value);

    /**
     * @brief Get int64_t value from prepared stmt.
     * 
     * @param columnIndex column index, >=1
     * @return int64_t value
     */
    int64_t ColumnInt(int columnIndex);
    std::string ColumnText(int columnIndex);
};

#endif