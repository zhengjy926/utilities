/**
  ******************************************************************************
  * @file        : mutex_example.c
  * @author      : ZJY
  * @version     : V1.0
  * @date        : 2025-01-27
  * @brief       : 互斥锁使用示例
  * @attention   : 本文件仅作为参考,不参与编译
  ******************************************************************************
  */

#include "mutex.h"
#include <stdio.h>

/* ============================================================================
 * 示例1: 使用互斥锁保护共享资源 (RTOS环境)
 * ============================================================================ */

static mutex_t resource_mutex;
static int shared_counter = 0;

void example_mutex_protect_resource(void)
{
    /* 创建互斥锁 */
    if (mutex_create(&resource_mutex) != 0) {
        printf("Failed to create mutex\n");
        return;
    }
    
    /* 访问共享资源 */
    if (mutex_lock(&resource_mutex) == 0) {
        /* 临界区代码 */
        shared_counter++;
        printf("Counter: %d\n", shared_counter);
        
        /* 释放互斥锁 */
        mutex_unlock(&resource_mutex);
    }
    
    /* 清理 */
    mutex_delete(&resource_mutex);
}

/* ============================================================================
 * 示例2: 使用中断锁保护链表操作 (所有环境)
 * ============================================================================ */

typedef struct node {
    int data;
    struct node *next;
} node_t;

static node_t *list_head = NULL;

void example_irq_lock_protect_list(void)
{
    node_t *new_node = (node_t*)malloc(sizeof(node_t));
    new_node->data = 42;
    
    /* 关中断保护链表操作 */
    irq_lock_t level = irq_lock();
    
    new_node->next = list_head;
    list_head = new_node;
    
    irq_unlock(level);
}

/* ============================================================================
 * 示例3: 使用临界区宏 (推荐方式)
 * ============================================================================ */

void example_critical_guard_macro(void)
{
    /* 方式1: 使用IRQ_GUARD宏 */
    IRQ_GUARD() {
        /* 这里的代码受中断保护 */
        list_head = NULL;
    }
    /* 自动恢复中断状态 */
    
    /* 方式2: 使用CRITICAL_GUARD宏 */
    CRITICAL_GUARD() {
        /* 这里的代码在临界区中 */
        shared_counter = 0;
    }
}

/* ============================================================================
 * 示例4: 使用互斥锁宏 (RTOS环境)
 * ============================================================================ */

void example_mutex_guard_macro(void)
{
    static mutex_t my_mutex;
    mutex_create(&my_mutex);
    
    /* 使用MUTEX_GUARD宏 */
    MUTEX_GUARD(my_mutex) {
        /* 这里的代码受互斥锁保护 */
        shared_counter++;
        printf("Protected counter: %d\n", shared_counter);
    }
    /* 自动释放互斥锁 */
}

/* ============================================================================
 * 示例5: 使用自旋锁 (SMP系统或极短时间保护)
 * ============================================================================ */

static spinlock_t my_spinlock = SPINLOCK_INITIALIZER;

void example_spinlock_usage(void)
{
    /* 方式1: 手动加锁解锁 */
    spinlock_lock(&my_spinlock);
    /* 极短的临界区代码 */
    shared_counter++;
    spinlock_unlock(&my_spinlock);
    
    /* 方式2: 使用宏 */
    SPINLOCK_GUARD(my_spinlock) {
        shared_counter++;
    }
}

/* ============================================================================
 * 示例6: 超时等待互斥锁
 * ============================================================================ */

void example_mutex_timeout(void)
{
    static mutex_t timeout_mutex;
    mutex_create(&timeout_mutex);
    
    /* 等待1秒 */
    int ret = mutex_lock_timeout(&timeout_mutex, 1000);
    if (ret == 0) {
        printf("Mutex acquired\n");
        /* 临界区代码 */
        mutex_unlock(&timeout_mutex);
    } else if (ret == -ETIMEDOUT) {
        printf("Mutex timeout\n");
    }
}

/* ============================================================================
 * 示例7: 非阻塞尝试加锁
 * ============================================================================ */

void example_mutex_trylock(void)
{
    static mutex_t trylock_mutex;
    mutex_create(&trylock_mutex);
    
    /* 尝试获取锁,不阻塞 */
    if (mutex_trylock(&trylock_mutex) == 0) {
        printf("Mutex acquired immediately\n");
        /* 临界区代码 */
        mutex_unlock(&trylock_mutex);
    } else {
        printf("Mutex is busy, skip this time\n");
    }
}

/* ============================================================================
 * 示例8: 在中断和任务中共享数据 (混合环境)
 * ============================================================================ */

static volatile uint32_t isr_counter = 0;

void example_isr_task_shared_data(void)
{
    /* 任务中访问共享数据 */
    irq_lock_t level = irq_lock();
    uint32_t counter = isr_counter;
    irq_unlock(level);
    
    printf("ISR counter: %u\n", counter);
}

/* 中断服务程序 */
void EXTI0_IRQHandler(void)
{
    /* 中断中修改共享数据 */
    isr_counter++;
}

/* ============================================================================
 * 示例9: 设备驱动中的锁使用
 * ============================================================================ */

typedef struct {
    mutex_t dev_mutex;      /* 保护设备状态 */
    spinlock_t reg_lock;    /* 保护寄存器访问 */
    int state;
} device_driver_t;

void example_device_driver_lock(device_driver_t *dev)
{
    /* 保护设备状态(可睡眠上下文) */
    mutex_lock(&dev->dev_mutex);
    dev->state = 1;
    mutex_unlock(&dev->dev_mutex);
    
    /* 保护寄存器访问(中断上下文) */
    spinlock_lock(&dev->reg_lock);
    /* 读写寄存器 */
    spinlock_unlock(&dev->reg_lock);
}

/* ============================================================================
 * 示例10: 适配现有代码 - device.c 中的使用
 * ============================================================================ */

#include "my_list.h"

typedef struct {
    char name[16];
    /* ... */
    list_t node;
} device_t;

static LIST_HEAD(device_list);

int device_register(device_t *dev, const char *name)
{
    /* 使用中断锁保护全局链表 */
    irq_lock_t level = irq_lock();
    list_add_tail(&dev->node, &device_list);
    irq_unlock(level);
    
    return 0;
}

device_t* device_find(const char *name)
{
    device_t *dev = NULL;
    list_t *node = NULL;
    
    /* 使用中断锁保护链表遍历 */
    irq_lock_t level = irq_lock();
    
    list_for_each(node, &device_list) {
        dev = list_entry(node, device_t, node);
        if (dev && strcmp(dev->name, name) == 0) {
            irq_unlock(level);
            return dev;
        }
    }
    
    irq_unlock(level);
    return NULL;
}

/* ============================================================================
 * 示例11: 不同环境下的编译
 * ============================================================================ */

/*
 * 编译配置:
 * 
 * 1. 裸机环境:
 *    gcc -c mutex.c -o mutex.o
 *    (不定义任何RTOS宏)
 * 
 * 2. FreeRTOS环境:
 *    gcc -c mutex.c -DUSING_FREERTOS -I./FreeRTOS/include -o mutex.o
 * 
 * 3. RT-Thread环境:
 *    gcc -c mutex.c -DRT_USING_MUTEX -I./rt-thread/include -o mutex.o
 * 
 * 4. CMSIS-RTOS环境:
 *    gcc -c mutex.c -DCMSIS_OS_H_ -I./CMSIS/RTOS2/Include -o mutex.o
 */

/* ============================================================================
 * 最佳实践建议
 * ============================================================================ */

/*
 * 1. 选择合适的锁类型:
 *    - mutex_lock:     可睡眠上下文,保护较长时间(>100us)的临界区
 *    - irq_lock:       所有上下文,保护极短时间(<10us)的临界区
 *    - spinlock:       SMP系统或需要忙等待的场景
 * 
 * 2. 避免死锁:
 *    - 按相同顺序获取多个锁
 *    - 持有锁时不要长时间阻塞
 *    - 使用超时机制
 * 
 * 3. 性能优化:
 *    - 临界区越小越好
 *    - 避免在临界区内调用耗时函数
 *    - 使用读写锁(如需要)
 * 
 * 4. 调试技巧:
 *    - 启用 MUTEX_DEBUG_ENABLE
 *    - 启用 MUTEX_ENABLE_DEADLOCK_DETECT
 *    - 使用静态分析工具
 */

