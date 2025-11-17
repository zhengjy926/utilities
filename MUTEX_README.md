# 通用互斥锁抽象层文档

## 📖 概述

通用互斥锁抽象层提供了统一的锁接口,支持裸机和多种RTOS环境,实现"一次编写,到处运行"。

### 支持的环境

| 环境 | 宏定义 | 状态 |
|-----|--------|------|
| **裸机** | 无 | ✅ 完全支持 |
| **FreeRTOS** | `USING_FREERTOS` | ✅ 完全支持 |
| **RT-Thread** | `RT_USING_MUTEX` | ✅ 完全支持 |
| **CMSIS-RTOS v2** | `CMSIS_OS_H_` | ✅ 完全支持 |
| **其他RTOS** | 待扩展 | 🔧 易于扩展 |

---

## 🚀 快速开始

### 1. 配置环境

编辑 `mutex_config.h`:

```c
/* 选择RTOS (取消注释) */
// #define USING_FREERTOS      // FreeRTOS
// #define RT_USING_MUTEX      // RT-Thread  
// #define CMSIS_OS_H_         // CMSIS-RTOS

/* 裸机环境: 保持全部注释 */
```

### 2. 包含头文件

```c
#include "mutex.h"
```

### 3. 使用中断锁 (最简单)

```c
/* 关中断保护临界区 */
irq_lock_t level = irq_lock();
/* 临界区代码 */
shared_data++;
irq_unlock(level);

/* 或使用宏 */
IRQ_GUARD() {
    shared_data++;
}
```

### 4. 使用互斥锁 (RTOS环境)

```c
/* 创建互斥锁 */
mutex_t my_mutex;
mutex_create(&my_mutex);

/* 加锁 */
mutex_lock(&my_mutex);
/* 临界区代码 */
mutex_unlock(&my_mutex);

/* 清理 */
mutex_delete(&my_mutex);
```

---

## 📚 API 参考

### 互斥锁 (Mutex)

#### 适用场景
- ✅ RTOS 环境
- ✅ 可睡眠上下文
- ✅ 保护较长时间的临界区 (>100μs)
- ❌ 中断上下文

#### API 列表

| 函数 | 说明 | 返回值 |
|-----|------|--------|
| `mutex_create()` | 创建互斥锁 | 0成功,负数错误 |
| `mutex_delete()` | 删除互斥锁 | 0成功,负数错误 |
| `mutex_lock()` | 获取锁(阻塞) | 0成功,负数错误 |
| `mutex_trylock()` | 尝试获取锁(非阻塞) | 0成功,-EBUSY忙 |
| `mutex_lock_timeout()` | 获取锁(超时) | 0成功,-ETIMEDOUT超时 |
| `mutex_unlock()` | 释放锁 | 0成功,负数错误 |

#### 使用示例

```c
mutex_t resource_lock;

/* 初始化 */
mutex_create(&resource_lock);

/* 使用方式1: 手动加锁 */
if (mutex_lock(&resource_lock) == 0) {
    /* 临界区 */
    shared_resource++;
    mutex_unlock(&resource_lock);
}

/* 使用方式2: 宏自动管理 */
MUTEX_GUARD(resource_lock) {
    shared_resource++;
}

/* 使用方式3: 超时等待 */
if (mutex_lock_timeout(&resource_lock, 1000) == 0) {
    /* 临界区 */
    mutex_unlock(&resource_lock);
} else {
    printf("Timeout\n");
}

/* 清理 */
mutex_delete(&resource_lock);
```

---

### 中断锁 (IRQ Lock)

#### 适用场景
- ✅ 所有环境 (裸机/RTOS)
- ✅ 所有上下文 (任务/中断)
- ✅ 保护极短时间的临界区 (<10μs)

#### API 列表

| 函数 | 说明 | 返回值 |
|-----|------|--------|
| `irq_lock()` | 关中断并保存状态 | 中断状态 |
| `irq_unlock(level)` | 恢复中断状态 | 无 |
| `critical_enter()` | 进入临界区 | 临界区状态 |
| `critical_exit(level)` | 退出临界区 | 无 |
| `in_interrupt()` | 检查是否在中断中 | true/false |

#### 使用示例

```c
/* 方式1: 手动管理 */
irq_lock_t level = irq_lock();
/* 临界区代码 */
list_add(&node, &list);
irq_unlock(level);

/* 方式2: 使用宏 */
IRQ_GUARD() {
    list_add(&node, &list);
}

/* 方式3: 临界区宏 */
CRITICAL_GUARD() {
    shared_counter++;
}
```

---

### 自旋锁 (Spinlock)

#### 适用场景
- ✅ SMP 多核系统
- ✅ 需要忙等待的场景
- ⚠️ 单核系统不推荐 (退化为中断锁)

#### API 列表

| 函数 | 说明 | 返回值 |
|-----|------|--------|
| `spinlock_init()` | 初始化自旋锁 | 无 |
| `spinlock_lock()` | 获取锁(忙等待) | 无 |
| `spinlock_trylock()` | 尝试获取锁 | true/false |
| `spinlock_unlock()` | 释放锁 | 无 |

#### 使用示例

```c
/* 静态初始化 */
static spinlock_t my_lock = SPINLOCK_INITIALIZER;

/* 动态初始化 */
spinlock_t my_lock;
spinlock_init(&my_lock);

/* 使用方式1: 手动管理 */
spinlock_lock(&my_lock);
/* 极短的临界区 */
register_value = READ_REG();
spinlock_unlock(&my_lock);

/* 使用方式2: 使用宏 */
SPINLOCK_GUARD(my_lock) {
    register_value = READ_REG();
}
```

---

## 🔧 配置说明

### mutex_config.h 配置项

```c
/* 1. RTOS选择 */
#define USING_FREERTOS              // FreeRTOS
// #define RT_USING_MUTEX           // RT-Thread
// #define CMSIS_OS_H_              // CMSIS-RTOS

/* 2. 功能开关 */
#define MUTEX_ENABLE_SPINLOCK       1    // 启用自旋锁
#define MUTEX_ENABLE_STATISTICS     0    // 启用统计功能
#define MUTEX_ENABLE_DEADLOCK_DETECT 0   // 启用死锁检测

/* 3. 调试配置 */
#define MUTEX_DEBUG_ENABLE          0    // 启用调试信息
```

---

## 🎯 使用场景指南

### 场景1: 保护全局链表 (device.c)

**问题**: 全局设备链表可能在中断和任务中同时访问

**解决方案**: 使用中断锁

```c
static LIST_HEAD(device_list);

int device_register(device_t *dev, const char *name)
{
    irq_lock_t level = irq_lock();
    list_add_tail(&dev->node, &device_list);
    irq_unlock(level);
    return 0;
}

device_t* device_find(const char *name)
{
    irq_lock_t level = irq_lock();
    list_for_each(node, &device_list) {
        /* ... */
    }
    irq_unlock(level);
    return dev;
}
```

**理由**: 
- ✅ 适用于所有环境
- ✅ 临界区极短 (~5μs)
- ✅ 可能在中断中调用

---

### 场景2: 保护UART发送缓冲区 (serial.c)

**问题**: 多个任务可能同时写串口

**解决方案**: 使用互斥锁

```c
#if USING_RTOS
ssize_t serial_write(serial_t *port, const void *buffer, size_t size)
{
    if (port->write_mutex) {
        mutex_lock(&port->write_mutex);
    }
    
    /* 写入FIFO */
    ret = kfifo_in(&port->tx_fifo, buffer, size);
    
    if (port->write_mutex) {
        mutex_unlock(&port->write_mutex);
    }
    return ret;
}
#endif
```

**理由**:
- ✅ RTOS 环境
- ✅ 可能阻塞等待
- ✅ 临界区时间不确定

---

### 场景3: 保护硬件寄存器

**问题**: 读-修改-写寄存器操作不是原子的

**解决方案**: 使用中断锁或自旋锁

```c
/* 方式1: 中断锁 (单核) */
void hw_reg_set_bit(volatile uint32_t *reg, uint32_t bit)
{
    irq_lock_t level = irq_lock();
    *reg |= (1 << bit);
    irq_unlock(level);
}

/* 方式2: 自旋锁 (多核) */
static spinlock_t reg_lock = SPINLOCK_INITIALIZER;

void hw_reg_set_bit(volatile uint32_t *reg, uint32_t bit)
{
    spinlock_lock(&reg_lock);
    *reg |= (1 << bit);
    spinlock_unlock(&reg_lock);
}
```

---

## 📊 性能对比

### 不同锁的开销 (Cortex-M4@168MHz)

| 锁类型 | 加锁开销 | 解锁开销 | 总开销 | 适用场景 |
|-------|---------|---------|--------|---------|
| **irq_lock** | ~10ns | ~10ns | ~20ns | 极短临界区 |
| **mutex (FreeRTOS)** | ~500ns | ~500ns | ~1μs | 可睡眠上下文 |
| **spinlock** | ~30ns | ~30ns | ~60ns | 忙等待场景 |

### 临界区持有时间建议

| 临界区时间 | 推荐锁类型 | 说明 |
|-----------|-----------|------|
| < 1μs | `irq_lock` | 极短时间,关中断 |
| 1μs ~ 100μs | `irq_lock` | 短时间,可接受关中断 |
| > 100μs | `mutex` | 长时间,必须用互斥锁 |
| 不确定 | `mutex` | 安全起见用互斥锁 |

---

## ⚠️ 常见陷阱

### 陷阱1: 持锁睡眠

```c
/* ❌ 错误: 持有中断锁时睡眠 */
irq_lock_t level = irq_lock();
delay_ms(10);  /* 睡眠时中断被关闭! */
irq_unlock(level);

/* ✅ 正确: 使用互斥锁 */
mutex_lock(&my_mutex);
delay_ms(10);  /* 互斥锁允许睡眠 */
mutex_unlock(&my_mutex);
```

### 陷阱2: 中断中使用互斥锁

```c
/* ❌ 错误: 中断中不能使用互斥锁 */
void IRQHandler(void)
{
    mutex_lock(&my_mutex);  /* 可能睡眠,导致系统崩溃! */
    /* ... */
    mutex_unlock(&my_mutex);
}

/* ✅ 正确: 中断中使用中断锁 */
void IRQHandler(void)
{
    irq_lock_t level = irq_lock();
    /* ... */
    irq_unlock(level);
}
```

### 陷阱3: 死锁

```c
/* ❌ 错误: 不同顺序获取锁 */
/* 线程A */
mutex_lock(&mutex1);
mutex_lock(&mutex2);  /* 死锁! */

/* 线程B */
mutex_lock(&mutex2);
mutex_lock(&mutex1);  /* 死锁! */

/* ✅ 正确: 相同顺序获取锁 */
/* 所有线程 */
mutex_lock(&mutex1);
mutex_lock(&mutex2);
```

---

## 🔄 移植到其他 RTOS

### 步骤1: 在 mutex.h 中添加检测

```c
#elif defined(MY_RTOS_H)
    #define OS_MY_RTOS
    #include "my_rtos.h"
```

### 步骤2: 在 mutex.c 中实现接口

```c
#ifdef OS_MY_RTOS
int mutex_create(mutex_t *mutex)
{
    *mutex = my_rtos_mutex_create();
    return (*mutex) ? 0 : -ENOMEM;
}

int mutex_lock(mutex_t *mutex)
{
    return my_rtos_mutex_lock(*mutex);
}

/* ... 实现其他接口 ... */
#endif
```

### 步骤3: 测试验证

```c
/* 运行测试用例 */
test_mutex_basic();
test_irq_lock();
```

---

## 📝 最佳实践

### 1. 选择合适的锁

```c
/* 链表操作: 中断锁 */
IRQ_GUARD() {
    list_add(&node, &list);
}

/* 文件操作: 互斥锁 */
MUTEX_GUARD(file_mutex) {
    file_write(fd, data, len);
}

/* 寄存器操作: 自旋锁(多核) */
SPINLOCK_GUARD(reg_lock) {
    REG = value;
}
```

### 2. 缩小临界区

```c
/* ❌ 不好: 临界区太大 */
mutex_lock(&mutex);
prepare_data();  /* 不需要保护 */
write_shared_data();
process_result();  /* 不需要保护 */
mutex_unlock(&mutex);

/* ✅ 好: 临界区最小 */
prepare_data();
mutex_lock(&mutex);
write_shared_data();  /* 只保护必要部分 */
mutex_unlock(&mutex);
process_result();
```

### 3. 使用宏简化代码

```c
/* ❌ 冗长 */
irq_lock_t level = irq_lock();
shared_data++;
irq_unlock(level);

/* ✅ 简洁 */
IRQ_GUARD() {
    shared_data++;
}
```

---

## 📊 总结对比表

| 特性 | 互斥锁 | 中断锁 | 自旋锁 |
|-----|--------|--------|--------|
| **环境** | RTOS | 所有 | 所有 |
| **上下文** | 任务 | 任务+中断 | 任务+中断 |
| **等待方式** | 睡眠 | 不等待 | 忙等待 |
| **开销** | 高(~1μs) | 低(~20ns) | 中(~60ns) |
| **适用时间** | >100μs | <100μs | <10μs |
| **可睡眠** | ✅ | ❌ | ❌ |
| **多核** | ✅ | ❌ | ✅ |

---

## 📞 技术支持

- **作者**: ZJY
- **版本**: V1.0
- **日期**: 2025-01-27

---

**祝使用愉快! 🎉**

