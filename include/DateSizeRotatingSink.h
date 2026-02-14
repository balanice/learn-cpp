#pragma once

#include <spdlog/sinks/base_sink.h>
#include <filesystem>
#include <fstream>
#include <mutex>

namespace spdlog {
namespace sinks {

class date_size_rotating_sink_mt : public spdlog::sinks::base_sink<std::mutex> {
public:
    // dir: directory where logs are written
    // base_name: prefix for log files (actual filename: base_name-YYYY-MM-DD[-N].log)
    // max_size: rotate when file >= max_size bytes
    date_size_rotating_sink_mt(const std::filesystem::path &dir, const std::string &base_name, std::size_t max_size = 1048576);
    ~date_size_rotating_sink_mt() override;

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;
    void flush_() override;

private:
    std::filesystem::path dir_;
    std::string base_name_;
    std::size_t max_size_;

    std::ofstream ofs_;
    std::string current_date_;
    unsigned int index_ = 0;
    std::filesystem::path current_path_;

    void reopen_file_();
    static std::string today_str_();
    std::uintmax_t current_file_size_() const;
};

} // namespace sinks
} // namespace spdlog
