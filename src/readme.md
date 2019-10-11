# vessel-concurrency/src

基于C++的线程安全的工具容器库源码（# gb2312编码）。

## 目录

```
Werkzeug/  -- 基础工具库
    SpinLock/ -- 自旋锁 √
        SpinLock -- 非公平自旋锁 √
        TicketSpinLock -- 公平自旋锁（返回排队号） √
        TicketLocalSpinLock -- 公平自旋锁（不返回排队号）√
        MCSLock -- 公平本地自旋锁
        ReentrantSpinLock -- 可重入自旋锁
    ReadWriteMutex/ -- 读写锁 √
        rwmutex -- 读优先的读写锁 √
        wrmutex -- 写优先的读写锁 √
        fair_rwmutex -- 读写公平的读写锁 √
    Semaphore -- 信号量
    AtomicPointer 
        atomic_shared_ptr -- 线程安全的shared_ptr
    SmartPointer
    
Vessel/ -- 容器库
    Stack/ -- 栈
        ConcurrentArrayStack -- 数组栈 √
        ConcurrentLinkedStack -- 链表栈 √
        ConcurrentLockFreeStack -- 无锁栈
        ConcurrentBlockingStack -- 链表阻塞栈 √
    Queue/ --队列
        ConcurrentArrayQueue -- 环形数组单向队列 √
        ConcurrentLinkedQueue -- 链表单向队列 √
        ConcurrentLockFreeStack -- 无锁单向队列
        LinkedBlockingQueue -- 链表阻塞队列 √
        ArrayBlockingQueue -- 环形数组阻塞队列 √ 
    Vector/ -- 数组
        CopyOnWriteArrayList -- 写时复制数组容器√
    Set/ -- 集合
        ConcurrentLinkedSet -- 基于链表的集合  √
        ConcurrentStrongHashSet -- 强一致性哈希集合（分段互斥锁） √
        ConcurrentWeakHashSet -- 弱一致性哈希集合（分段无锁读，或分段读写锁）√    
    Map/ -- 
 ```

