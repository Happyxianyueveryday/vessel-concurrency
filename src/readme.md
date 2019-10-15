# vessel-concurrency/src

基于C++的线程安全的工具容器库源码（# GB2312/GB18030编码）。

GC，Werkzurg等几个部分都是为Vessel，也即最终的容器设计服务的，在容器设计中，部分容器使用了上述的工具。

## 目录

```
GC/ -- 垃圾回收
    MarkSweepGC -- 基于标记-清除(Mark&Sweep)算法的简单C++垃圾回收器
    
Werkzeug/  -- 基础工具库
    SpinLock/ -- 自旋锁 
        SpinLock -- 非公平自旋锁 √
        TicketSpinLock -- 公平自旋锁（返回排队号） √
        TicketLocalSpinLock -- 公平自旋锁（不返回排队号）√
    ReadWriteMutex/ -- 读写锁 
        rwmutex -- 读优先的读写锁 √
        wrmutex -- 写优先的读写锁 √
        fair_rwmutex -- 读写公平的读写锁 √
    AtomicPointer -- 原子标记指针
        AtomicStampPointer -- 版本号指针 √
        AtomicMarkablePointer -- 标志物指针 √
    SmartPointer/ -- 智能指针 
        shared_ptr -- 共享对象所有权的智能指针 √
        unique_ptr -- 独享对象所有权的智能指针 √
        weak_ptr -- 无对象所有权的弱智能指针 √

Vessel/ -- 容器库
    Stack/ -- 栈
        ConcurrentArrayStack -- 数组栈 √
        ConcurrentLinkedStack -- 链表栈 √
        ConcurrentLockFreeStack -- 无锁栈 √
        ConcurrentBlockingStack -- 链表阻塞栈 √
    Queue/ --队列
        ConcurrentArrayQueue -- 环形数组单向队列 √
        ConcurrentLinkedQueue -- 链表单向队列 √
        ConcurrentLockFreeStack -- 无锁单向队列
        LinkedBlockingQueue -- 链表阻塞队列 √
        ArrayBlockingQueue -- 环形数组阻塞队列 √ 
    Vector/ -- 数组
        CopyOnWriteArrayList -- 写时复制数组容器 √
    Set/ -- 集合
        ConcurrentLinkedSet -- 基于链表的集合  √
        ConcurrentStrongHashSet -- 强一致性哈希集合（分段互斥锁） √
        ConcurrentWeakHashSet -- 弱一致性哈希集合（分段无锁读，或分段读写锁）√    
    Map/ -- 
 ```
 
 ### 待完成时间线
```
MCSLock -- 公平本地自旋锁 
ReentrantSpinLock -- 可重入自旋锁
Semaphore -- 信号量
AtomicPointer -- 线程安全智能指针
atomic_shared_ptr -- 线程安全的shared_ptr
```


