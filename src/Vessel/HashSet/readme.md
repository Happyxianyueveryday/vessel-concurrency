# src/Vessel/HashSet: 并发哈希集合

### 目录

```
  ConcurrentLinkedSet -- 基于链表的哈希集合
  ConcurrentStrongHashSet -- 基于互斥锁的强一致性哈希集合（有锁读，有锁写，jdk1.7标准）
  ConcurrentWeakHashSet -- 基于HashEntry的弱一致性哈希集合（无锁读，有锁写，jdk1.7标准）
```

