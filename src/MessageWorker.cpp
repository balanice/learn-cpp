#include "MessageWorker.h"

#include "database.h"
#include "MyCrypt2.h"

#include <mutex>
#include <optional>
#include <spdlog/spdlog.h>
#include <openssl/evp.h>
#include <sstream>

// Base64 decode helper using OpenSSL EVP_DecodeBlock
static std::vector<unsigned char> base64Decode(const std::string& in) {
    if (in.empty()) return {};
    std::vector<unsigned char> out(in.size());
    int olen = EVP_DecodeBlock(out.data(), reinterpret_cast<const unsigned char*>(in.data()), static_cast<int>(in.size()));
    if (olen < 0) return {};
    // EVP_DecodeBlock may include padding bytes; adjust length based on '=' padding
    size_t padding = 0;
    if (!in.empty() && in.back() == '=') padding++;
    if (in.size() >= 2 && in[in.size()-2] == '=') padding++;
    size_t trueLen = static_cast<size_t>(olen) - padding;
    out.resize(trueLen);
    return out;
}

void MessageWorker::setWorkKey(const std::vector<unsigned char>& workKey) {
    std::lock_guard<std::mutex> lock(mutex_);
    workKey_ = workKey;
    decryptEnabled_ = !workKey_.empty();
}

void MessageWorker::processMessage(const Message &msg) {
    std::visit([this](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, WriteMessage>) {
            // 处理写消息
            spdlog::info("Writing key: {}, value: {}", arg.key, arg.value);
            db_.InsertMessage(arg); // 示例调用，实际应传递参数
        } else if constexpr (std::is_same_v<T, ReportMessage>) {
            // 处理上报消息
            spdlog::info("Reporting from tag: {}", arg.tag);
            auto messages = db_.QueryMessages();
            for (const auto& m : messages) {
                // 如果是加密格式：ENC:iv_b64:tag_b64:cipher_b64
                if (m.value.rfind("ENC:", 0) == 0 && decryptEnabled_) {
                    try {
                        std::string payload = m.value.substr(4);
                        std::istringstream iss(payload);
                        std::string iv_b64, tag_b64, cipher_b64;
                        if (std::getline(iss, iv_b64, ':') && std::getline(iss, tag_b64, ':') && std::getline(iss, cipher_b64)) {
                            auto iv = base64Decode(iv_b64);
                            auto tag = base64Decode(tag_b64);
                            auto cipher = base64Decode(cipher_b64);
                            if (!iv.empty() && !tag.empty() && !cipher.empty()) {
                                std::string keyStr(reinterpret_cast<const char*>(workKey_.data()), workKey_.size());
                                std::string ivStr(reinterpret_cast<const char*>(iv.data()), iv.size());
                                std::vector<unsigned char> plain(cipher.size() + 16);
                                int ret = gcm_decrypt(cipher.data(), static_cast<int>(cipher.size()), tag.data(), keyStr, ivStr, plain.data());

                                // int ret = gcm_decrypt(cipher.data(), static_cast<int>(cipher.size()), tag.data(), keyStr, ivStr, plain.data());
                                if (ret >= 0) {
                                    std::string decrypted(reinterpret_cast<const char*>(plain.data()), static_cast<size_t>(ret));
                                    spdlog::info("Message - id: {}, key: {}, value (decrypted): {}", m.id, m.key, decrypted);
                                } else {
                                    spdlog::error("Failed to decrypt message id {} key {} - tag verification failed", m.id, m.key);
                                }
                                continue;
                            }
                        }
                    } catch (const std::exception& e) {
                        spdlog::error("Decryption error for message id {}: {}", m.id, e.what());
                    }
                }

                // Default: show raw value
                spdlog::info("Message - id: {}, key: {}, value: {}", m.id, m.key, m.value);
            }
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

MessageWorker::MessageWorker() : stop_(false), db_() {
    worker_thread_ = std::thread(&MessageWorker::run, this);
    auto create = db_.CreateTable();
    if (!create) {
        spdlog::error("Failed to create necessary tables in database.");
    }
}

MessageWorker::~MessageWorker() {
    stop(false); // 析构时立即停止
}