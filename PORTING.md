# GD32VW55x WIFI 驱动软件包 - 移植说明

## 移植步骤清单

### ✅ 已完成

- [x] 创建软件包目录结构
- [x] 创建 package.json
- [x] 创建 Kconfig 配置文件
- [x] 创建 SConscript 构建脚本
- [x] 创建 README.md 文档
- [x] 创建移植层模板文件

### 📋 待完成（需要手动操作）

#### 1. 复制 SDK 源文件

从 `GD32VW55x_RELEASE_V1.0.3f` 复制以下文件到软件包：

```bash
# 1. wifi_manager 源码
复制: MSDK/wifi_manager/*.c, *.h
目标: packages/gd32vw55x-wifi/src/wifi_manager/

复制: MSDK/wifi_manager/wpas/*
目标: packages/gd32vw55x-wifi/src/wifi_manager/wpas/

# 2. macsw 接口头文件
复制: MSDK/macsw/export/*.h
目标: packages/gd32vw55x-wifi/src/macsw/export/

复制: MSDK/macsw/import/*.h
目标: packages/gd32vw55x-wifi/src/macsw/import/

# 3. 平台层
复制: MSDK/plf/src/*
目标: packages/gd32vw55x-wifi/src/plf/src/

注意：GD32VW55x_standard_peripheral 已存在于 packages/gd32-riscv-series-latest/GD32VW55x/ 下
构建脚本会自动引用该路径，无需重复复制

# 4. RTOS 适配层
复制: MSDK/rtos/rtos_wrapper/*
目标: packages/gd32vw55x-wifi/src/rtos/rtos_wrapper/

# 5. 工具库
复制: MSDK/util/src/*
目标: packages/gd32vw55x-wifi/src/util/src/

复制: MSDK/util/include/*
目标: packages/gd32vw55x-wifi/src/util/include/

# 6. 预编译库
复制: MSDK/lib/libwifi.a
目标: packages/gd32vw55x-wifi/lib/

复制: MSDK/lib/libwpas.a
目标: packages/gd32vw55x-wifi/lib/

复制: MSDK/lib/librf.a
目标: packages/gd32vw55x-wifi/lib/

# 7. ROM 符号表
复制: ROM-EXPORT/symbol/rom_symbol_m.gcc
目标: packages/gd32vw55x-wifi/rom_export/symbol/

复制: ROM-EXPORT/halcomm/*
目标: packages/gd32vw55x-wifi/rom_export/halcomm/

复制: ROM-EXPORT/mbedtls-2.17.0-rom/
目标: packages/gd32vw55x-wifi/rom_export/mbedtls-2.17.0-rom/
```

#### 2. 实现移植层接口

需要完善以下文件的实现：

**port/wifi_netif_port.c**
- [ ] 实现 `wifi_netdev_init()` - 网络接口初始化
- [ ] 实现 `wifi_netdev_open()` - 打开网络接口
- [ ] 实现 `wifi_netdev_close()` - 关闭网络接口
- [ ] 实现 `wifi_netdev_set_dhcp()` - DHCP 控制
- [ ] 实现 `wifi_status_changed_callback()` - WIFI 状态回调
- [ ] 对接 LwIP netif 与 RT-Thread netdev

**port/wifi_mem_port.c**
- [ ] 实现 `wifi_mem_heap_init()` - 内存堆初始化
- [ ] 确认是否需要特殊堆区域
- [ ] 验证内存分配兼容性

**samples/wifi_sta_sample.c**
- [ ] 根据实际 API 调整函数调用
- [ ] 添加错误处理
- [ ] 测试连接流程

**samples/wifi_ap_sample.c**
- [ ] 根据实际 API 调整函数调用
- [ ] 实现 AP 配置
- [ ] 测试 AP 模式

#### 3. 验证和调试

- [ ] 检查头文件路径是否正确
- [ ] 验证预编译库兼容性（工具链版本）
- [ ] 测试 RTOS 接口适配（wrapper_os）
- [ ] 验证中断处理
- [ ] 测试内存管理
- [ ] 完整功能测试

#### 4. 文档完善

- [ ] 更新 README.md 添加实际 API 说明
- [ ] 添加详细的配置说明
- [ ] 添加常见问题解答
- [ ] 添加移植注意事项

## 注意事项

### 1. 内存配置
确保系统堆内存足够（建议 >= 128KB）：
```c
// rtconfig.h
#define RT_HEAP_SIZE (128 * 1024)
```

### 2. 线程栈配置
WIFI 线程需要较大栈空间：
```c
// Kconfig
config GD32VW55X_WIFI_TASK_STACK_SIZE
    default 4096
```

### 3. 中断优先级
在 board.h 或相关配置中设置 WIFI 中断优先级。

### 4. LwIP 配置
确保 LwIP 已正确配置：
```c
#define RT_USING_LWIP
#define RT_LWIP_MEM_ALIGNMENT  4
#define RT_LWIP_PBUF_NUM       16
#define RT_LWIP_TCP_PCB_NUM    8
```

### 5. 工具链兼容性
预编译库需要与工具链兼容：
- 架构: RISC-V RV32IMAC
- ABI: ilp32
- 工具链: GCC

## 目录结构说明

```
gd32vw55x-wifi/
├── inc/                    公共头文件（可选）
├── src/                    源代码
│   ├── wifi_manager/       WIFI 管理层 [从 SDK 复制]
│   ├── macsw/             MAC 层接口 [从 SDK 复制]
│   ├── plf/src/           平台层源码 [从 SDK 复制]
│   ├── rtos/              RTOS 适配 [从 SDK 复制]
│   └── util/              工具库 [从 SDK 复制]
├── lib/                   预编译库 [从 SDK 复制]
├── rom_export/            ROM 符号 [从 SDK 复制]
├── port/                  RT-Thread 适配层 [已创建模板]
├── samples/               示例代码 [已创建模板]
├── package.json           软件包配置 [已创建]
├── Kconfig                配置选项 [已创建]
├── SConscript            构建脚本 [已创建，自动引用外设库]
└── README.md             使用文档 [已创建]
```

注意：GD32VW55x_standard_peripheral 使用 gd32-riscv-series-latest 包中的，无需复制

## 下一步

1. 按照上述步骤复制 SDK 文件
2. 完善移植层接口实现
3. 编译测试
4. 功能验证

如有问题，请参考 README.md 或联系维护者。
