#ifndef SIMPLE_GC_H
#define SIMPLE_GC_H
#include <stddef.h>

typedef struct garbage_collector * GarbageCollector;

typedef void (*gc_event_callback)(void* object);    // �ص���������


/**
 * msgc_create: ����Mark&Sweep gc
 * param nobjects: ��ʼ�����Ķ������
 * param size: ���������С
 * param on_mark: ��Ƕ���Ļص�����
 * param on_collect: �ռ�����Ļص�����
 * param on_destroy: ���ٶ���Ļص�����
 */
GarbageCollector msgc_create(size_t nobjects,
                           size_t size,
                           gc_event_callback on_mark,
                           gc_event_callback on_collect,
                           gc_event_callback on_destroy);

/**
 * msgc_add: ��Mark&Sweep gc�����Ӷ�������
 * param gc: Mark&Sweep gc����
 * param nobjects: ���ӵĶ������
 */
void  msgc_add     (GarbageCollector gc , size_t nobjects);

/**
 * msgc_malloc: ����һ�����󲢷��ظö����ָ��
 * param gc: Mark&Sweep gc����
 */
void* msgc_malloc   (GarbageCollector gc);

/**
 * msgc_root: ��Mark&Sweep gc������һ��������Ϊ�����
 * param gc: Mark&Sweep gc����
 * param object: ��������
 */
void  msgc_root    (GarbageCollector gc , void* object);

/**
 * msgc_unroot: ��Mark&Sweep gc���Ƴ�һ����������
 * param gc: Mark&Sweep gc����
 * param object: ��������
 */
void  msgc_unroot  (GarbageCollector gc , void* object);

/**
 * msgc_protect: ����Mark&Sweep gc��һ����������
 * param gc: Mark&Sweep gc����
 * param object: ��������
 */
void  msgc_protect (GarbageCollector gc , void** object);

/**
 * msgc_expose: ����Mark&Sweep gc��һ����������
 * param gc: Mark&Sweep gc����
 * param object: ��������
 */
void  msgc_expose  (GarbageCollector gc , size_t n);

/**
 * msgc_free: �ͷ�Mark&Sweep gc�е����ж���
 * param gc: Mark&Sweep gc����
 */
void  msgc_free    (GarbageCollector* gc);

/**
 * msgc_mark: ��ǿɴ�Ķ���  
 */
void  msgc_mark    (gc_event_callback cont, void* object);

/**
 * msgc_collect: �ռ����ɴ���� 
 */
void  msgc_collect (GarbageCollector gc);
#endif
