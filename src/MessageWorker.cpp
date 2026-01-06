#include "MessageWorker.h"
#include <mutex>
#include <optional>
#include <spdlog/spdlog.h>

void MessageWorker::processMessage(const Message &msg) {
    std::visit([this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, WriteMessage>) {
            // 处理写消息
            spdlog::info("Writing key: {}, value: {}", arg.key, arg.value);
            // db_.write(arg.key, arg.value); // 假设 LocalDatabase 有 write 方法
        } else if constexpr (std::is_same_v<T, ReportMessage>) {
            // 处理上报消息
            spdlog::info("Reporting from tag: {}", arg.tag);
            // db_.report(arg.tag); // 假设 LocalDatabase 有 report 方法
        }
    }, msg);
}

void MessageWorker::run() {
    while (true) {
        std::optional<Message> msg_opt;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] {
                return stop_.load() || !queue_.empty();
            });

            if (stop_.load()) {
                if (!drain_ || queue_.empty()) break;
            }

            if (!queue_.empty()) {
                msg_opt = std::move(queue_.front());
                queue_.pop();
            } else if (stop_.load() && drain_) {
                break;
            }
        }

        if (msg_opt) {
            processMessage(*msg_opt);
        }
    }
    spdlog::info("[Worker] Thread exited.");
}

void MessageWorker::submit(Message msg) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stop_.load(std::memory_order_acquire)) return;
    queue_.push(std::move(msg));
    cv_.notify_one();
}

void MessageWorker::stop(bool drain) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stop_.load()) return;
        stop_.store(true);
        drain_ = drain;
    }
    cv_.notify_all();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

MessageWorker::MessageWorker() : stop_(false) {
    worker_thread_ = std::thread(&MessageWorker::run, this);
}

MessageWorker::~MessageWorker() {
    stop(false); // 析构时立即停止
}