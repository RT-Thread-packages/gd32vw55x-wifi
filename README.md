# GD32VW55x WIFI Driver Package

[English](README.md) | [中文](README_zh.md)

## 1. 简介

GD32VW55x WIFI 驱动软件包是为 GD32VW55x 系列芯片的 WIFI 功能提供的 RT-Thread 驱动包。本软件包支持 802.11 b/g/n 协议，提供 Station 和 SoftAP 模式，支持 WPA/WPA2/WPA3 安全协议。

### 1.1 主要特性

- **WIFI 模式**
  - Station (STA) 模式 - 连接到 AP
  - SoftAP 模式 - 作为热点
  - 并发模式 (STA+AP)

- **安全协议**
  - WPA/WPA2-PSK
  - WPA3-SAE (可选)
  - WPS (可选)

- **网络功能**
  - 基于 LwIP 网络协议栈
  - DHCP Client/Server
  - DNS 客户端
  - TCP/UDP 协议

### 1.2 目录结构

```
gd32vw55x-wifi
├── inc                     # 公共头文件
├── src                     # 源代码
│   ├── wifi_manager        # WIFI 管理层
│   │   └── wpas            # WPA Supplicant 接口层
│   ├── macsw               # MAC 层接口
│   │   ├── export          # 导出头文件
│   │   └── import          # 导入头文件
│   ├── plf                 # 平台适配层
│   ├── rtos                # RTOS 适配层
│   └── util                # 工具库
├── lib                     # 预编译库
│   ├── libwifi.a           # WIFI MAC 层库
│   ├── libwpas.a           # WPA Supplicant 库
│   └── librf.a             # RF 射频库
├── rom_export              # ROM 导出符号
├── port                    # RT-Thread 移植层
├── samples                 # 示例代码
├── package.json            # 软件包描述
├── Kconfig                 # 配置选项
├── SConscript              # 构建脚本
└── README.md               # 使用说明
```

## 2. 如何使用

### 2.1 依赖

- RT-Thread 4.0+
- LwIP 2.0+
- GD32 RISC-V BSP

### 2.2 获取软件包

软件包位于 BSP 的 `packages/gd32vw55x-wifi` 目录下。

### 2.3 移植步骤

#### 步骤 1: 复制 SDK 文件

从 GD32VW55x_RELEASE_V1.0.3f SDK 复制以下内容：

```bash
# 1. 复制 wifi_manager 源码
复制 MSDK/wifi_manager/* 到 src/wifi_manager/

# 2. 复制 macsw 头文件
复制 MSDK/macsw/export/* 到 src/macsw/export/
复制 MSDK/macsw/import/* 到 src/macsw/import/

# 3. 复制平台层
复制 MSDK/plf/src/* 到 src/plf/src/
注意：GD32VW55x_standard_peripheral 已存在于 packages/gd32-riscv-series-latest/GD32VW55x/ 下，无需重复复制

# 4. 复制 RTOS 适配层
复制 MSDK/rtos/rtos_wrapper/* 到 src/rtos/rtos_wrapper/

# 5. 复制工具库
复制 MSDK/util/src/* 到 src/util/src/
复制 MSDK/util/include/* 到 src/util/include/

# 6. 复制预编译库
复制 MSDK/lib/libwifi.a 到 lib/
复制 MSDK/lib/libwpas.a 到 lib/
复制 MSDK/lib/librf.a 到 lib/

# 7. 复制 ROM 符号
复制 ROM-EXPORT/symbol/rom_symbol_m.gcc 到 rom_export/symbol/
复制 ROM-EXPORT/halcomm/* 到 rom_export/halcomm/
复制 ROM-EXPORT/mbedtls-2.17.0-rom/ 到 rom_export/
```

#### 步骤 2: 配置软件包

使用 menuconfig 配置：

```bash
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] Enable GD32VW55x WIFI support --->
            [*] Enable Station Mode
            [ ] Enable SoftAP Mode
            Security Configuration --->
                [*] Enable WPA/WPA2 Security
                [ ] Enable WPA3 Security
            [*] Enable WIFI Sample Code
```

#### 步骤 3: 实现移植层

在 `port/` 目录下实现以下适配：

1. **wifi_netif_port.c** - 网络接口适配
2. **wifi_mem_port.c** - 内存管理适配
3. **wifi_rtos_port.c** - RTOS 接口扩展（如需要）

#### 步骤 4: 编译

```bash
scons
```

### 2.4 配置选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| PKG_USING_GD32VW55X_WIFI | 启用 WIFI 支持 | n |
| GD32VW55X_WIFI_STATION_MODE | Station 模式 | y |
| GD32VW55X_WIFI_SOFTAP_MODE | SoftAP 模式 | n |
| GD32VW55X_WIFI_WPA_SECURITY | WPA/WPA2 安全 | y |
| GD32VW55X_WIFI_WPA3_SECURITY | WPA3 安全 | n |
| GD32VW55X_WIFI_TASK_STACK_SIZE | 任务栈大小 | 4096 |
| GD32VW55X_WIFI_DEBUG | 调试输出 | n |

## 3. API 说明

### 3.1 初始化

```c
int wifi_init(void);
```

### 3.2 Station 模式

```c
// 扫描 AP
int wifi_scan(void);

// 连接 AP
int wifi_connect(const char *ssid, const char *password);

// 断开连接
int wifi_disconnect(void);

// 获取连接状态
int wifi_get_status(wifi_status_t *status);
```

### 3.3 SoftAP 模式

```c
// 启动 AP
int wifi_start_ap(const char *ssid, const char *password, uint8_t channel);

// 停止 AP
int wifi_stop_ap(void);
```

## 4. 示例代码

### 4.1 Station 模式示例

```c
#include <rtthread.h>
#include "wifi_management.h"

void wifi_sta_example(void)
{
    // 初始化 WIFI
    wifi_init();

    // 扫描 AP
    wifi_scan();
    rt_thread_mdelay(3000);

    // 连接 AP
    wifi_connect("YourSSID", "YourPassword");

    // 等待连接
    rt_thread_mdelay(5000);

    // 获取 IP 地址
    wifi_status_t status;
    wifi_get_status(&status);
    rt_kprintf("IP: %s\n", status.ip);
}
```

### 4.2 SoftAP 模式示例

```c
#include <rtthread.h>
#include "wifi_management.h"

void wifi_ap_example(void)
{
    // 初始化 WIFI
    wifi_init();

    // 启动 AP
    wifi_start_ap("GD32VW_AP", "12345678", 6);

    rt_kprintf("AP started\n");
}
```

## 5. 注意事项

1. **内存需求**：WIFI 功能需要较大的堆内存（建议 >= 128KB）
2. **中断优先级**：WIFI 中断优先级需要合理配置
3. **线程栈**：WIFI 相关线程需要足够的栈空间
4. **ROM 符号**：必须正确链接 ROM 符号表
5. **预编译库**：确保预编译库与工具链兼容（GCC RISC-V）

## 6. 已知问题

- [ ] 需要完成 RT-Thread 网络接口适配
- [ ] 需要验证内存管理适配
- [ ] 需要测试 WPA3 功能

## 7. 联系方式

- 维护：RT-Thread 社区
- 主页：https://github.com/RT-Thread/rt-thread
- 邮箱：support@rt-thread.com

## 8. 许可证

BSD-3-Clause License

Copyright (c) 2023, GigaDevice Semiconductor Inc.
