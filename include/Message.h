#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <variant>

struct WriteMessage {
    std::string key;
    std::string value;
};

struct ReportMessage {
    std::string tag; // 用于标识上报来源（可选）
};

using Message = std::variant<WriteMessage, ReportMessage>;

#endif // MESSAGE_H