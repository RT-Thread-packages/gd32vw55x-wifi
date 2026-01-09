/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-25     RT-Thread    the first version
 */

#include <rtthread.h>

#ifdef PKG_USING_GD32VW55X_WIFI

#define DBG_TAG "wifi.port.mem"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/**
 * @brief WIFI 内存管理适配层
 *
 * GD32 SDK 使用自定义内存管理函数，需要适配到 RT-Thread 的内存管理
 *
 * SDK 中的内存函数:
 * - os_malloc()  -> rt_malloc()
 * - os_free()    -> rt_free()
 * - os_calloc()  -> rt_calloc()
 * - os_realloc() -> rt_realloc()
 *
 * 注意: SDK 可能使用特殊的堆区域，需要根据实际情况调整
 */

/**
 * @brief 添加额外的堆区域（如果需要）
 *
 * GD32 SDK 在某些情况下使用扩展堆区域:
 * sys_add_heap_region(0x20048000, 0x8000);
 *
 * 可以使用 RT-Thread 的多堆管理或 memheap 功能
 */
int wifi_mem_heap_init(void)
{
    // TODO: 如果 SDK 需要特殊的堆区域，在这里初始化

    // 示例: 添加扩展堆（根据实际需求调整地址和大小）
    // #ifdef RT_USING_MEMHEAP
    // static struct rt_memheap wifi_heap;
    // rt_memheap_init(&wifi_heap, "wifi_heap", (void *)0x20048000, 0x8000);
    // #endif

    LOG_I("WIFI memory heap initialized");

    return RT_EOK;
}

/**
 * @brief 获取内存使用统计（可选）
 */
void wifi_mem_info(void)
{
    rt_uint32_t total, used, max_used;

    rt_memory_info(&total, &used, &max_used);

    rt_kprintf("Memory Info:\n");
    rt_kprintf("  Total  : %d bytes\n", total);
    rt_kprintf("  Used   : %d bytes\n", used);
    rt_kprintf("  Max    : %d bytes\n", max_used);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(wifi_mem_info, show wifi memory usage);
#endif

#endif /* PKG_USING_GD32VW55X_WIFI */
