# src/WerkZurg/SpinLock: 自旋锁

提供cpp11原生线程库中缺失的自旋锁实现方案。

## 目录

```
SpinLock/ -- 自旋锁
    SpinLock -- 非公平的自旋锁
    TicketSpinLock -- 公平自旋锁（暴露排队号）
    TicketLocalSpinLock -- 公平自旋锁（不暴露排队号）
    CLHLock -- 公平本地自旋锁
    MCSLock -- 公平本地自旋锁
```

## 附注和选用建议
> 1. SpinLock是最基本的基于CAS操作的自旋锁，仅适用于不要求公平的环境下。
> 2. TicketSpinLock在SpinLock基础上加入了排队号和服务号，从而实现了公平的自旋锁，但是会返回排队号给线程，因此暴露了一定风险。
> 3. TicketLocalSpinLock在SpinLock基础上加入了排队号和服务号，从而实现了公平的自旋锁，使用thread_local的方案存储排队号以避免TicketSpinLock所具有的风险。每个线程持有自己的排队号变量，但所有线程共享读写一个服务号。因此推荐用于单CPU环境下，在多CPU环境下，因为多个线程的自旋操作都在一个服务号上，每次自旋都必须在多个处理器缓存之间进行缓存同步，会导致系统总线繁忙，从而遇到性能瓶颈。
> 4. CLHLock是基于链表的可扩展、高性能、公平的自旋锁。适用于SMP架构（多CPU共享内存）的环境下。CLHLock通过链表的形式，每个线程持有一个状态结点，所有的自旋操作都在线程本地变量上进行，从而避免了基于排队号和服务号的自旋锁在多CPU情况下的性能瓶颈。
> 5. MCSLock
