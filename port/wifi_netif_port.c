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
#include <rtdevice.h>
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include <netdev.h>

#ifdef PKG_USING_GD32VW55X_WIFI

#include "wifi_management.h"
#include "wifi_netif.h"

#define DBG_TAG "wifi.port.netif"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* RT-Thread netdev 对接 GD32 WIFI 网络接口 */

/**
 * @brief 将 GD32 WIFI 网络接口适配到 RT-Thread netdev 框架
 *
 * 本文件需要实现以下功能:
 * 1. 创建 netdev 设备并注册
 * 2. 对接 LwIP netif 与 RT-Thread netdev
 * 3. 处理 WIFI 状态变化回调
 * 4. 实现网络接口的启动/停止控制
 */

static struct netdev *wifi_netdev = RT_NULL;

/**
 * @brief 网络接口启动回调
 */
static int wifi_netdev_set_up(struct netdev *netdev)
{
    LOG_I("WIFI netdev set up");
    // TODO: 调用 GD32 WIFI 初始化函数
    return RT_EOK;
}

/**
 * @brief 网络接口关闭回调
 */
static int wifi_netdev_set_down(struct netdev *netdev)
{
    LOG_I("WIFI netdev set down");
    // TODO: 调用 GD32 WIFI 去初始化函数
    return RT_EOK;
}

/**
 * @brief 设置 DHCP 状态
 */
static int wifi_netdev_set_dhcp(struct netdev *netdev, rt_bool_t is_enabled)
{
    LOG_I("WIFI set DHCP: %s", is_enabled ? "enabled" : "disabled");
    // TODO: 实现 DHCP 启用/禁用
    return RT_EOK;
}

/**
 * @brief 设置 IP 地址
 */
static int wifi_netdev_set_addr_info(struct netdev *netdev, ip_addr_t *ip_addr,
                                      ip_addr_t *netmask, ip_addr_t *gw)
{
    LOG_I("WIFI set addr info");
    // TODO: 设置静态 IP 地址
    return RT_EOK;
}

/**
 * @brief 设置 DNS 服务器
 */
static int wifi_netdev_set_dns_server(struct netdev *netdev, uint8_t dns_num, ip_addr_t *dns_server)
{
    LOG_I("WIFI set DNS server");
    // TODO: 设置 DNS 服务器地址
    return RT_EOK;
}

/**
 * @brief PING 测试
 */
#ifdef RT_USING_FINSH
static int wifi_netdev_ping(struct netdev *netdev, const char *host,
                            size_t data_len, uint32_t timeout, struct netdev_ping_resp *ping_resp, rt_bool_t isbind)
{
    LOG_I("WIFI ping");
    // TODO: 实现 PING 功能（可选）
    return RT_EOK;
}
#endif

/**
 * @brief 设置默认网络接口
 */
static int wifi_netdev_set_default(struct netdev *netdev)
{
    LOG_I("WIFI set default");
    // TODO: 设置为默认网络接口
    return RT_EOK;
}

/* netdev 操作函数表 */
static const struct netdev_ops wifi_netdev_ops =
{
    .set_up         = wifi_netdev_set_up,
    .set_down       = wifi_netdev_set_down,
    .set_dhcp       = wifi_netdev_set_dhcp,
    .set_addr_info  = wifi_netdev_set_addr_info,
    .set_dns_server = wifi_netdev_set_dns_server,
#ifdef RT_USING_FINSH
    .ping           = wifi_netdev_ping,
#endif
    .set_default    = wifi_netdev_set_default,
};

/**
 * @brief WIFI 状态变化回调
 *
 * @param event WIFI 事件类型
 * @param data 事件数据
 */
void wifi_status_changed_callback(uint32_t event, void *data)
{
    switch (event)
    {
    case WIFI_MGMT_EVENT_CONNECT_SUCCESS:
        LOG_I("WIFI connected");
        if (wifi_netdev)
        {
            netdev_low_level_set_link_status(wifi_netdev, RT_TRUE);
        }
        break;

    case WIFI_MGMT_EVENT_DISCONNECT:
        LOG_I("WIFI disconnected");
        if (wifi_netdev)
        {
            netdev_low_level_set_link_status(wifi_netdev, RT_FALSE);
        }
        break;

    case WIFI_MGMT_EVENT_DHCP_SUCCESS:
        LOG_I("WIFI got IP");
        if (wifi_netdev)
        {
            // TODO: 更新 netdev IP 地址信息
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 注册 WIFI netdev 设备
 */
int wifi_netdev_register(void)
{
    wifi_netdev = netdev_get_by_name("wlan0");
    if (wifi_netdev == RT_NULL)
    {
        LOG_E("Get netdev failed");
        return -RT_ERROR;
    }

    /* 设置 netdev 操作函数 - 直接赋值而非使用 netdev_set_ops */
    wifi_netdev->ops = &wifi_netdev_ops;

    /* 设置默认网卡 */
    netdev_set_default(wifi_netdev);

    LOG_I("WIFI netdev registered");

    return RT_EOK;
}

#endif /* PKG_USING_GD32VW55X_WIFI */
