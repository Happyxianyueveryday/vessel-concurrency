#ifndef SIMPLE_GC_H
#define SIMPLE_GC_H
#include <stddef.h>

typedef struct garbage_collector * GarbageCollector;

typedef void (*gc_event_callback)(void* object);    // 回调函数定义


/**
 * msgc_create: 创建Mark&Sweep gc
 * param nobjects: 初始创建的对象个数
 * param size: 单个对象大小
 * param on_mark: 标记对象的回调函数
 * param on_collect: 收集对象的回调函数
 * param on_destroy: 销毁对象的回调函数
 */
GarbageCollector msgc_create(size_t nobjects,
                           size_t size,
                           gc_event_callback on_mark,
                           gc_event_callback on_collect,
                           gc_event_callback on_destroy);

/**
 * msgc_add: 向Mark&Sweep gc中增加对象数量
 * param gc: Mark&Sweep gc对象
 * param nobjects: 增加的对象个数
 */
void  msgc_add     (GarbageCollector gc , size_t nobjects);

/**
 * msgc_malloc: 分配一个对象并返回该对象的指针
 * param gc: Mark&Sweep gc对象
 */
void* msgc_malloc   (GarbageCollector gc);

/**
 * msgc_root: 向Mark&Sweep gc中增加一个对象作为根结点
 * param gc: Mark&Sweep gc对象
 * param object: 根结点对象
 */
void  msgc_root    (GarbageCollector gc , void* object);

/**
 * msgc_unroot: 向Mark&Sweep gc中移除一个根结点对象
 * param gc: Mark&Sweep gc对象
 * param object: 根结点对象
 */
void  msgc_unroot  (GarbageCollector gc , void* object);

/**
 * msgc_protect: 锁定Mark&Sweep gc中一个根结点对象
 * param gc: Mark&Sweep gc对象
 * param object: 根结点对象
 */
void  msgc_protect (GarbageCollector gc , void** object);

/**
 * msgc_expose: 解锁Mark&Sweep gc中一个根结点对象
 * param gc: Mark&Sweep gc对象
 * param object: 根结点对象
 */
void  msgc_expose  (GarbageCollector gc , size_t n);

/**
 * msgc_free: 释放Mark&Sweep gc中的所有对象
 * param gc: Mark&Sweep gc对象
 */
void  msgc_free    (GarbageCollector* gc);

/**
 * msgc_mark: 标记可达的对象  
 */
void  msgc_mark    (gc_event_callback cont, void* object);

/**
 * msgc_collect: 收集不可达对象 
 */
void  msgc_collect (GarbageCollector gc);
#endif
