#include "MessageWorker.h"
#include "Message.h"
#include "spdlog/spdlog.h"


void InitLog()
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    // change log pattern
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
}

int main(int, char **)
{
    InitLog();

    spdlog::info("Starting MessageWorker test...");
    MessageWorker worker;
    worker.submit(WriteMessage{"key1", "value1"});
    worker.submit(ReportMessage{"tagA"});
    worker.submit(WriteMessage{"key2", "value2"});
    std::this_thread::sleep_for(std::chrono::seconds(2));
    worker.stop();
    spdlog::info("MessageWorker test finished.");
}
