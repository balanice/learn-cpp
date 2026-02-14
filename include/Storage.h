#pragma once

#include "Config.h"
#include "MessageWorker.h"
#include "KeyManager.h"

#include <string>
#include <vector>
#include <memory>

class Storage {
public:
    // Load config from filePath (if exists) or use defaults from Config
    explicit Storage(const Config& cfg);

    // Store a key/value pair. If encryption is enabled in the config, value will be encrypted
    // with KeyManager's work key and stored via MessageWorker.
    void store(const std::string& key, const std::string& value);

    // Expose MessageWorker stop for clean shutdown
    void stop(bool drain = true) { worker_.stop(drain); }

    // trigger a report (MessageWorker will query DB and display messages)
    void report() { worker_.submit(ReportMessage{"storage_report"}); }

private:
    bool encrypt_ = false;
    std::string storagePath_;
    std::vector<unsigned char> workKey_;
    MessageWorker worker_;
};
