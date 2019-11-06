# src/Vessel

c++常用并发容器。

# 目录

```
Vessel/ -- 常用并发容器
    Stack/ -- 栈
        ConcurrentArrayStack -- 数组栈 √
        ConcurrentLinkedStack -- 链表栈 √
        ConcurrentLockFreeStack -- 无锁栈 √
        ConcurrentBlockingStack -- 链表阻塞栈 √
    Queue/ --队列
        ConcurrentArrayQueue -- 环形数组单向队列 √
        ConcurrentLinkedQueue -- 链表单向队列 √
        ConcurrentLockFreeStack -- 无锁单向队列 √
        LinkedBlockingQueue -- 链表阻塞队列 √
        ArrayBlockingQueue -- 环形数组阻塞队列 √ 
    Vector/ -- 数组
        CopyOnWriteArrayList -- 写时复制数组容器 √
    Set/ -- 集合
        ConcurrentStrongHashSet -- 强一致性并发哈希集合（分段互斥锁） √
        ConcurrentWeakHashSet -- 弱一致性并发哈希集合（分段无锁读，或分段读写锁）√    
        ConcurrentLinkedSet -- 基于链表的集合  √
    Map/ -- 映射
        ConcurrentStrongHashMap -- 强一致性并发哈希映射（分段互斥锁） √
        ConcurrentWeakHashSet -- 弱一致性并发哈希集合（分段无锁读，或分段读写锁） 
```
