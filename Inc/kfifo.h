/**
 * @file        kfifo.h
 * @author      ZJY
 * @version     V1.0
 * @date        20xx-xx-xx
 * @brief       Kernel FIFO implementation header file
 * @details     This file contains the definition of a kernel-style FIFO buffer
 *              implementation with thread-safe operations.
 * @attention   None
 * @history
 * | Version | Description |
 * |---------|-------------|
 * | V1.0    | Initial implementation |
 */
 #ifndef KFIFO_H
 #define KFIFO_H
 
 #ifdef __cplusplus
  extern "C" {
 #endif /* __cplusplus */
 
 /* Includes ------------------------------------------------------------------*/
 #include <stddef.h>
 #include <stdint.h>
 #include <stdbool.h>
 
 /* Exported typedef ----------------------------------------------------------*/
 /*
  * 编译器级屏障 (适用所有架构的基线防御)
  * 阻止编译器优化器重排访存指令。
  */
 #define KFIFO_COMPILER_BARRIER() __asm__ volatile("": : :"memory")
     
 /*
  * 2. 硬件级屏障抽象配置
  * 根据你使用的目标架构，通过编译宏打开对应的硬件指令。
  */
 
 #if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_8A__) || defined(__aarch64__)
     // 高端 ARM (如 Cortex-A 多核)，存在乱序执行和复杂的多核缓存
     // 必须使用最强的 DMB (Data Memory Barrier) 指令
     #define KFIFO_WMB() __asm__ volatile("dmb st" : : : "memory") // 写屏障
     #define KFIFO_RMB() __asm__ volatile("dmb ld" : : : "memory") // 读屏障
 
 #elif defined(__arm__) || defined(__ARM_ARCH)
     // 中低端 ARM Cortex-M (M0/M3/M4/M7)
     // Cortex-M 是顺序执行架构。在单核且不涉及 DMA 高速并发时，编译器屏障通常足够。
     // 但如果有带 Cache 的 Cortex-M7，或者极度保守的 Class C 规范，建议加上 DMB。
     #define KFIFO_WMB() __asm__ volatile("dmb" : : : "memory")
     #define KFIFO_RMB() __asm__ volatile("dmb" : : : "memory")
 
 #elif defined(__riscv)
     // RISC-V 架构
     // 使用 FENCE 指令
     #define KFIFO_WMB() __asm__ volatile("fence w,  w" : : : "memory")
     #define KFIFO_RMB() __asm__ volatile("fence r,  r" : : : "memory")
 
 #elif defined(__x86_64__) || defined(_M_X64)
     // x86 / x64 架构 (常用于在 PC 端跑白盒测试)
     // x86 是强内存模型 (TSO)，写自带屏障，通常编译器屏障即可，但 sfence/lfence 更严谨
     #define KFIFO_WMB() __asm__ volatile("sfence" : : : "memory")
     #define KFIFO_RMB() __asm__ volatile("lfence" : : : "memory")
 
 #else
     // 默认回退 (Fallback)：对于未知的简单单片机 (如 8051, AVR)
     // 它们只有顺序执行，没有 Cache，编译器屏障就足够了
     #define KFIFO_WMB() KFIFO_COMPILER_BARRIER()
     #define KFIFO_RMB() KFIFO_COMPILER_BARRIER()
 #endif
 
 /**
  * @brief FIFO buffer structure
  * @details This structure represents a circular buffer (FIFO) with support for
  *          variable element sizes and thread-safe operations.
  */
 typedef struct
 {
     volatile uint32_t in;     /**< Input index */
     volatile uint32_t out;    /**< Output index */
     uint32_t          mask;   /**< Size mask (size - 1) */
     uint32_t          esize;  /**< Element size in bytes */
     uint8_t           *data;  /**< Pointer to buffer data */
 } Kfifo_t;
 
 
 static inline uint32_t Kfifo_Avail(Kfifo_t *fifo)
 {
     return (fifo->mask + 1) - (fifo->in - fifo->out);
 }
 
 static inline uint32_t Kfifo_Len(Kfifo_t *fifo)
 {
     return fifo->in - fifo->out;
 }
 
 /* Exported function prototypes ----------------------------------------------*/
 int32_t  Kfifo_Init     (Kfifo_t *fifo, void *buffer, uint32_t size, size_t esize);
 uint32_t Kfifo_In       (Kfifo_t *fifo, const void *buf, uint32_t len);
 uint32_t Kfifo_Out      (Kfifo_t *fifo, void *buf, uint32_t len);
 uint32_t Kfifo_OutPeek  (Kfifo_t *fifo, void *buf, uint32_t len);
 uint32_t Kfifo_OutLinear(Kfifo_t *fifo, uint32_t *tail, uint32_t n);
 void     Kfifo_SkipCount(Kfifo_t *fifo, uint32_t cnt);
 
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
 
 #endif /* KFIFO_H */
 