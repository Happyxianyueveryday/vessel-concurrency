# src/Werkzurg

对C++原生多线程库的补充工具包。

### 目录
```
Werkzeug/  -- 基础工具库
    SpinLock/ -- 自旋锁 
        SpinLock -- 非公平自旋锁 √
        TicketSpinLock -- 公平自旋锁（返回排队号） √
        TicketLocalSpinLock -- 公平自旋锁（不返回排队号）√
        MCSLock -- 公平本地自旋锁 
        ReentrantSpinLock -- 可重入自旋锁
    ReadWriteMutex/ -- 读写锁 
        rwmutex -- 读优先的读写锁 √
        wrmutex -- 写优先的读写锁 √
        fair_rwmutex -- 读写公平的读写锁 √
    Semaphore -- 信号量
    AtomicPointer -- 线程安全智能指针
        atomic_shared_ptr -- 线程安全的shared_ptr
    SmartPointer/ -- 智能指针 
        shared_ptr -- 共享对象所有权的智能指针 √
        unique_ptr -- 独享对象所有权的智能指针 √
        weak_ptr -- 无对象所有权的弱智能指针 √
```
