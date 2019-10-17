#include "gc.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void* lmalloc(size_t n) 
{
     void* result = malloc(n);
     if (!result) {
          fputs("gc: out of memory.\n",stderr);
          abort();    // 分配失败
     }
     memset(result,0,n);
     return result;
}

typedef struct node * Node;
struct node 
{
     Node next; 
     char mark;
};

static Node node(size_t size, Node next) 
{
     Node n = lmalloc(size + sizeof *n); 
     n->next = next;
     n->mark = 0;
     return n;
}

typedef struct root * Root;
struct root 
{
     Root next;
     void* data;
};

static Root root(Root next, void* data) 
{
     Root r = lmalloc(sizeof *r);
     r->next = next;
     r->data = data;

     return r;
}

struct garbage_collector 
{
     Node free;              // 处于空闲的对象列表
     Node active;            // 正在使用的对象列表
     Root used_roots;        // 正在使用的根结点
     Root free_roots;        // 空闲根结点
     Root used_protected;    // 保护态占用结点
     Root free_protected;    // 保护态空闲结点
     gc_callback_fn on_mark;    // 标记对象回调
     gc_callback_fn on_collect; // 收集对象回调
     gc_callback_fn on_destroy; // 销毁对象回调
     size_t size;            // 单个对象大小
};

GC gc_create(size_t nobjects, size_t size, gc_callback_fn on_mark, gc_callback_fn on_collect, gc_callback_fn on_destroy) 
{
     GC gc = lmalloc(sizeof *gc);
     gc->on_mark = on_mark;
     gc->on_collect = on_collect;
     gc->on_destroy = on_destroy;
     gc->size = size;

     size_t i;
     for (i = 0; i < nobjects; i++)
          gc->free = node(size,gc->free);

     return gc;
}

static void delete_nodes(Node head, gc_callback_fn on_destroy) 
{
     Node cur = NULL;
     Node next = NULL;
     for (cur = head; cur ; cur = next) {
          next = cur->next;
          if (on_destroy)
               on_destroy(cur+1); 
          free(cur);
     }
}

static void delete_roots(Root head) {
     Root cur,next;
     
     for (cur = head; cur ; cur = next) {
          next = cur->next;
          free(cur);
     }
}

void gc_add(GC gc, size_t nobjects) 
{
     assert(gc);
     size_t i;
     for (i = 0; i < nobjects; i++)
          gc->free = node(gc->size,gc->free);
}

void gc_root(GC gc , void* object) 
{
     assert(gc);
     Root r = NULL;
     if (gc->free_roots) 
     { 
          r = gc->free_roots;
          r->next = gc->used_roots;
          gc->free_roots = gc->free_roots->next;
     }
     else 
     r = root(gc->used_roots,object);
     gc->used_roots = r;
}

void gc_unroot(GC gc , void* object) 
{
     assert(gc);
     Root cur,prev;
     for (cur = gc->used_roots, prev = NULL; cur; cur = cur->next) 
     {
          if (cur->data == object) 
          {
               if (prev)
                    prev->next = cur->next;
               else
                    gc->used_roots = cur->next;

               /* put it on free root list */
               cur->data = NULL;
               cur->next = gc->free_roots;
               gc->free_roots = cur;
          }
          prev = cur;
     }      
}
void gc_free(GC* gc) {
     if (!gc)
          return;
     if (!*gc)
          return;
     delete_nodes((*gc)->free,(*gc)->on_destroy);
     delete_nodes((*gc)->active,(*gc)->on_destroy);
     delete_roots((*gc)->used_roots);
     delete_roots((*gc)->free_roots);
     delete_roots((*gc)->used_protected);
     delete_roots((*gc)->free_protected);
     free(*gc);
     *gc = NULL;
}

void gc_protect(GC gc , void** object) 
{
     assert(gc);
     Root r = NULL;
     if (gc->free_protected) 
     { 
          r = gc->free_protected;
          r->next = gc->used_protected;
          gc->free_protected = gc->free_protected->next;
     }
     else 
     r = root(gc->used_protected,(void*)object);
     gc->used_protected = r;
}

void gc_expose(GC gc , size_t n) 
{
     assert(gc);
     while (n-->0 && gc->used_protected) 
     {
          Root r = gc->used_protected;
          gc->used_protected = r->next;
          r->next = gc->free_protected;
          gc->free_protected = r;
     }
}

static inline int is_marked(void* object) 
{
     Node n = (Node)(((char*)object) - sizeof(*n));
     return n->mark;
}

static inline void set_mark(void* object) 
{
     Node n = (Node)(((char*)object) - sizeof(*n));
     n->mark = 1;
}

static inline void unset_mark(void* object) 
{
     Node n = (Node)(((char*)object) - sizeof(*n));
     n->mark = 0;
}

void  gc_mark(gc_callback_fn cont, void* object) 
{
     if (!object || is_marked(object))
          return;
     set_mark(object);
     if (cont)
     cont(object);
}

static inline void collect(GC gc, Node n, Node prev) 
{
     assert(gc);
     assert(n);
     if (gc->on_collect) 
          gc->on_collect(n+1); 
     if (prev)
          prev->next = n->next;
     else 
          gc->active = n->next;
     n->next = gc->free;
     gc->free = n;
}

void gc_collect(GC gc) 
{
     assert(gc);
     Root cur;
     // 标记
     for (cur = gc->used_roots; cur ; cur = cur->next)
          gc_mark(gc->on_mark,cur->data);
     for (cur = gc->used_protected; cur; cur = cur->next)
          gc_mark(gc->on_mark,*(void**)cur->data);
     Node n,next,prev;
     prev = NULL;

     // 清除
     for (n = gc->active; n ; n = next)
     {
          next = n->next;
          if (n->mark) 
          {
               n->mark = 0;
               prev = n;
          }
          else
          collect(gc,n,prev);
     }
}

void* gc_malloc(GC gc) 
{
     assert(gc);
     if (!gc->free) gc_collect(gc); 
     if (!gc->free) return NULL; 

     Node n = gc->free;
     gc->free = gc->free->next;
     n->next = gc->active;
     gc->active = n;
     n->mark = 0;
     return (n+1); 
}
