#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) start++;
    auto end = s.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

Config::Config()
    : storagePath_("./data"), encrypt_(false) {}

const std::string& Config::getStoragePath() const { return storagePath_; }
void Config::setStoragePath(const std::string& path) { storagePath_ = path; }

bool Config::isEncrypted() const { return encrypt_; }
void Config::setEncrypted(bool v) { encrypt_ = v; }

bool Config::loadFromFile(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return false;

    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) continue;
        if (line.front() == '#') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        auto key = trim(line.substr(0, pos));
        auto val = trim(line.substr(pos + 1));
        if (key == "storage_path") {
            storagePath_ = val;
        } else if (key == "encrypt") {
            std::string low = val;
            std::transform(low.begin(), low.end(), low.begin(), [](unsigned char c){ return std::tolower(c); });
            encrypt_ = (low == "1" || low == "true" || low == "yes" || low == "on");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& path) const {
    std::ofstream ofs(path);
    if (!ofs.is_open()) return false;
    ofs << "# Config file\n";
    ofs << "storage_path=" << storagePath_ << "\n";
    ofs << "encrypt=" << (encrypt_ ? "1" : "0") << "\n";
    return true;
}
