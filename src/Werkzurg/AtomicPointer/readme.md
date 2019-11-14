## src/Werkzurg/AtomicPointer: 标志物指针

标志物指针。此部分目前慎用，因为c++对gc的支持问题，虽然功能正常，但是存在内存泄漏。待完成c++的gc后该部分会重新修复。

## 目录
```
AtomicPointer/ -- 标志物指针
    AtomicStampPointer -- 版本号指针
    AtomicMarkablePointer -- 标志变量指针
```
