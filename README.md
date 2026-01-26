# learn-cpp

My cpp samples. only tested on Arch Linux.

Rquirements:

`C++ 20`, `CMake3.5+`, `sqlite3`, `OpenSSL3.x`, `curl`, `spdlog`

## 数据库操作
使用 sqlite 操作数据
* database: 用于管理数据库，如数据库的创建，插入，删除等动作；
* StatementWrapper: 将 sqlite3_stmt 用 c++ 类包装起来, 易于使用, 由 C++ 类机制自动回收;

## 消息队列
MessageWorker: 使用固定消息类型，WriteMessage 为写入， ReportMessage 为读取后上报到后台；
Message: 定义消息模型。

## 加解密
使用openssl对存储在本地的数据进行加解密
1. 使用随机数生成rootKey；
2. 使用随机数生成gcm密钥，用rootKey对密钥进行加密，存储在本地；使用密钥对数据库中数据进行加解密；
3. 启动时候检查rootKey的有效期和完整性，如果不完整，则删除数据库中所有已加密数据和gcm密钥，重新生成rootKey和gcm密钥；