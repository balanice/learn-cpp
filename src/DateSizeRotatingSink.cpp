#include "DateSizeRotatingSink.h"

#include <spdlog/details/os.h>
#include <spdlog/details/null_mutex.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std::literals;

namespace spdlog {
namespace sinks {

static std::string format_date(std::time_t t) {
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string date_size_rotating_sink_mt::today_str_() {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    return format_date(tt);
}

date_size_rotating_sink_mt::date_size_rotating_sink_mt(const std::filesystem::path &dir, const std::string &base_name, std::size_t max_size)
    : dir_(dir), base_name_(base_name), max_size_(max_size)
{
    current_date_ = today_str_();
    index_ = 0;
    reopen_file_();
}

date_size_rotating_sink_mt::~date_size_rotating_sink_mt() {
    try { flush_(); } catch (...) {}
    if (ofs_.is_open()) ofs_.close();
}

std::uintmax_t date_size_rotating_sink_mt::current_file_size_() const {
    std::error_code ec;
    if (current_path_.empty()) return 0;
    auto sz = std::filesystem::file_size(current_path_, ec);
    return ec ? 0 : sz;
}

void date_size_rotating_sink_mt::reopen_file_() {
    if (!std::filesystem::exists(dir_)) {
        std::error_code ec;
        std::filesystem::create_directories(dir_, ec);
    }

    // construct filename: base_name-YYYY-MM-DD[-N].log
    std::ostringstream fname;
    fname << base_name_ << '-' << current_date_;
    if (index_ > 0) fname << '-' << index_;
    fname << ".log";

    current_path_ = dir_ / fname.str();

    // close previous
    if (ofs_.is_open()) ofs_.close();

    // open in append mode
    ofs_.open(current_path_, std::ios::out | std::ios::app | std::ios::binary);
    if (!ofs_.is_open()) {
        throw spdlog::spdlog_ex("Failed to open log file: " + current_path_.string());
    }
}

void date_size_rotating_sink_mt::sink_it_(const spdlog::details::log_msg &msg) {
    // Check date rotation
    auto today = today_str_();
    if (today != current_date_) {
        current_date_ = today;
        index_ = 0;
        reopen_file_();
    }

    // size-based rotation
    if (current_file_size_() >= max_size_) {
        ++index_;
        reopen_file_();
    }

    // format message and write
    memory_buf_t formatted;
    this->formatter_->format(msg, formatted);
    ofs_.write(formatted.data(), static_cast<std::streamsize>(formatted.size()));
    ofs_.put('\n');
}

void date_size_rotating_sink_mt::flush_() {
    if (ofs_.is_open()) ofs_.flush();
}

} // namespace sinks
} // namespace spdlog
