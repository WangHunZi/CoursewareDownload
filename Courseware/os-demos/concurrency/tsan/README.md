**ThreadSanitizer**: 通过寻找是否存在没有 happens-before 关系的不同线程、同一变量、至少一个是写的内存访问 (数据竞争)。这也称为 happens-before race。同时，TSAN 也不是万能的：触发 happens-before race 依然可能需要特定的线程调度。
