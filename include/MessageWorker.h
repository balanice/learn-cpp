#ifndef MESSAGEWORKER_H
#define MESSAGEWORKER_H

#include "Message.h"
#include "database.h"
#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>

class MessageWorker {
public:
    explicit MessageWorker();

    ~MessageWorker();

    void submit(Message msg);

    // 停止：drain=true 则处理完剩余消息；false 则立即退出
    void stop(bool drain = true);

private:
    void run();
    void processMessage(const Message& msg);

    std::thread worker_thread_;
    std::queue<Message> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stop_{false};
    bool drain_ = true;

    MyDatabase db_; // 每个 worker 拥有自己的 DB（或可设为全局）
};

#endif // MESSAGEWORKER_H