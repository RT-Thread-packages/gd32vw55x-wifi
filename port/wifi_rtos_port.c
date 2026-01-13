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

#define DBG_TAG "wifi.port.rtos"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/**
 * @brief RTOS 接口扩展
 *
 * GD32 SDK 的 rtos_wrapper 已经实现了大部分 RTOS 抽象层
 * 本文件用于提供额外的 RT-Thread 特定功能或修正
 *
 * 主要 RTOS 接口 (已在 wrapper_os.c 中实现):
 * - os_thread_create/delete
 * - os_sem_create/delete/wait/signal
 * - os_mutex_create/delete/lock/unlock
 * - os_queue_create/delete/send/recv
 * - os_timer_create/delete/start/stop
 * - os_malloc/free/calloc/realloc
 */

/**
 * @brief RTOS 适配层初始化
 */
int wifi_rtos_port_init(void)
{
    LOG_I("WIFI RTOS port initialized");

    // TODO: 如果需要额外的初始化，在这里添加

    return RT_EOK;
}

/**
 * @brief 验证 RTOS 接口兼容性（调试用）
 */
#ifdef GD32VW55X_WIFI_DEBUG
void wifi_rtos_test(void)
{
    rt_kprintf("RTOS Interface Test:\n");
    rt_kprintf("  RT-Thread Version: %d.%d.%d\n",
               RT_VERSION, RT_SUBVERSION, RT_REVISION);
    rt_kprintf("  Tick per second: %d\n", RT_TICK_PER_SECOND);
    rt_kprintf("  CPU frequency: %d Hz\n", SystemCoreClock);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(wifi_rtos_test, test rtos interface);
#endif

#endif /* GD32VW55X_WIFI_DEBUG */

#endif /* PKG_USING_GD32VW55X_WIFI */
