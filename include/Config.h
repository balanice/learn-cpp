#pragma once

#include <string>

class Config {
public:
    Config();

    const std::string& getStoragePath() const;
    void setStoragePath(const std::string& path);

    bool isEncrypted() const;
    void setEncrypted(bool v);

    // Load/save simple key=value config file
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;

private:
    std::string storagePath_;
    bool encrypt_;
};
