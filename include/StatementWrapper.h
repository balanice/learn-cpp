#ifndef STATEMENT_WRAPPER_H
#define STATEMENT_WRAPPER_H

#include <memory>
#include <sqlite3.h>
#include <string>

class StatementWrapper
{
private:
    std::shared_ptr<sqlite3> db;        // 保持 shared_ptr，确保 sqlite 实例存活
    std::shared_ptr<sqlite3_stmt> statement;
    std::string query;

    std::shared_ptr<sqlite3_stmt> prepareStmt();
    void check(const int ret) const;
public:
    explicit StatementWrapper(const std::shared_ptr<sqlite3>& database, const std::string& query);
    ~StatementWrapper() = default;
    sqlite3_stmt* getPreparedStatement() const;
    int Step();
    void Reset();
    void ClearBindings();

    /**
     * Bind text to index(>=1).
     */
    int Bind(int bindIndex, const std::string& text);
    int Bind(int bindIndex, const int64_t value);

    /**
     * @brief Get int64_t value from prepared stmt.
     * 
     * @param columnIndex column index, >=0
     * @return int64_t value
     */
    int64_t ColumnInt(int columnIndex);
    std::string ColumnText(int columnIndex);
};

#endif