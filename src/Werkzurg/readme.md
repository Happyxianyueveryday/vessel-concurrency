# src/Werkzurg

对C++原生多线程库的补充工具包。

### 目录
```
Werkzeug/  -- 基础工具库
    SpinLock/ -- 自旋锁 
        SpinLock -- 非公平自旋锁 √
        TicketSpinLock -- 公平自旋锁（返回排队号） √
        TicketLocalSpinLock -- 公平自旋锁（thread_local，不返回排队号）√
    ReadWriteMutex/ -- 读写锁 
        rwmutex -- 读优先的读写锁 √
        wrmutex -- 写优先的读写锁 √
        fair_rwmutex -- 读写公平的读写锁 √
    SmartPointer/ -- 智能指针 
        shared_ptr -- 共享对象所有权的智能指针 √
        unique_ptr -- 独享对象所有权的智能指针 √
        weak_ptr -- 无对象所有权的弱智能指针 √
    AtomicPointer/ -- 版本指针
        AtomicStampPointer -- 版本号指针 √
        AtomicMarkablePointer -- 标志指针 √
    CacheModel/ -- 缓存模型 
        FIFOCache -- 先进先出置换缓存 √
        LIFOCache -- 后进先出置换缓存 √
        LRUCache -- 最近最少使用置换缓存 √
        MRUCache -- 最近最常使用置换缓存 （预计11.7完成）
```
