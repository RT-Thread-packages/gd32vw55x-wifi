# GD32VW55x WIFI 软件包 - 配置编译指南

## 📋 环境要求

### 1. 必需工具

```bash
# 检查工具版本
python --version          # Python 3.x
scons --version          # SCons 构建工具
riscv-none-embed-gcc --version  # RISC-V GCC 工具链
```

### 2. 安装 SCons（如果未安装）

```bash
pip install scons
```

### 3. 配置工具链路径

编辑 `rtconfig.py`，确认 `EXEC_PATH` 指向正确的 RISC-V GCC 工具链：

```python
EXEC_PATH = r'C:/Program Files (x86)/GNU Tools RISC-V Embedded/8 2019.08-2/bin'
```

或者设置环境变量：
```bash
set RTT_EXEC_PATH=C:\your\toolchain\path\bin
```

---

## 🔧 配置步骤

### 步骤 1: 进入 BSP 目录

```bash
cd d:\rt-thread-main\rt-thread\bsp\gd32\risc-v\gd32vw553h-eval
```

### 步骤 2: 生成默认配置（首次）

```bash
# 如果是首次编译，先生成默认配置
scons --menuconfig
```

### 步骤 3: 启用 WIFI 软件包

在 menuconfig 中导航到：

```
RT-Thread online packages --->
    [*] Enable GD32VW55x WIFI support --->
        [*] Enable Station Mode              # 启用 STA 模式
        [ ] Enable SoftAP Mode               # 可选：AP 模式
        Security Configuration --->
            [*] Enable WPA/WPA2 Security     # WPA 安全
            [ ] Enable WPA3 Security         # 可选：WPA3
        [*] Enable WIFI Sample Code          # 启用示例代码
```

**关键配置项：**

| 配置项 | 说明 | 建议 |
|-------|------|------|
| PKG_USING_GD32VW55X_WIFI | 启用 WIFI 包 | ✓ 必须 |
| GD32VW55X_WIFI_STATION_MODE | Station 模式 | ✓ 建议 |
| GD32VW55X_WIFI_SOFTAP_MODE | SoftAP 模式 | 按需 |
| GD32VW55X_WIFI_WPA_SECURITY | WPA/WPA2 | ✓ 建议 |
| GD32VW55X_WIFI_TASK_STACK_SIZE | 任务栈大小 | 4096 |
| GD32VW55X_WIFI_USING_SAMPLE | 示例代码 | ✓ 测试用 |

### 步骤 4: 配置依赖项

确保以下选项已启用：

```
RT-Thread Kernel --->
    [*] Enable dynamic heap memory management

RT-Thread Components --->
    Network --->
        [*] Enable lwIP network stack
        [*] Enable netdev
        Device drivers --->
            [*] Using WiFi framework
```

### 步骤 5: 保存配置并退出

- 按 `Y` 保存配置
- 配置会保存到 `.config` 文件
- 自动生成 `rtconfig.h`

---

## 🔨 编译步骤

### 方法 1: 使用 SCons 编译

```bash
# 清理编译
scons -c

# 开始编译
scons -j8

# 查看详细输出
scons -j8 --verbose
```

### 方法 2: 使用 Python + SCons

```bash
python -m SCons -j8
```

### 编译选项

```bash
scons -j8              # 多线程编译（8线程）
scons --verbose        # 显示详细编译信息
scons -c               # 清理编译产物
scons --target=mdk5    # 生成 MDK5 工程（如需要）
```

---

## 🔍 编译过程检查

### 1. 观察编译输出

正常情况下应该看到：

```
Compiling src/wifi_manager/wifi_init.c
Compiling src/wifi_manager/wifi_management.c
Compiling src/wifi_manager/wifi_vif.c
...
Compiling src/plf/src/bsp.c
Compiling src/plf/src/gd32vw55x_platform.c
...
Linking libwifi.a
Linking libwpas.a
Linking librf.a
...
Linking rtthread.elf
```

### 2. 检查链接库

确认预编译库正确链接：

```
LINK(libwifi.a)
LINK(libwpas.a)
LINK(librf.a)
LINK(rom_symbol_m.gcc)
```

### 3. 检查生成文件

```bash
build/              # 编译中间文件
rtthread.elf        # 可执行文件
rtthread.bin        # 二进制文件
rtthread.map        # 内存映射文件
```

---

## ⚠️ 常见编译问题

### 问题 1: 找不到头文件

**错误信息：**
```
fatal error: wifi_management.h: No such file or directory
```

**解决方法：**
1. 检查 SConscript 中的 CPPPATH 配置
2. 确认文件已正确复制到 src 目录
3. 运行 `scons -c` 清理后重新编译

### 问题 2: 链接错误 - 未定义的引用

**错误信息：**
```
undefined reference to `wifi_init'
```

**解决方法：**
1. 确认预编译库在 lib/ 目录下
2. 检查 SConscript 中的 LIBS 和 LIBPATH 配置
3. 确认库文件与工具链兼容（RISC-V RV32IMAC）

### 问题 3: ROM 符号链接错误

**错误信息：**
```
cannot find rom_symbol_m.gcc
```

**解决方法：**
1. 检查 `rom_export/symbol/rom_symbol_m.gcc` 是否存在
2. 确认 SConscript 中 LOCAL_CFLAGS 的链接参数正确

### 问题 4: 内存不足

**错误信息：**
```
region 'RAM' overflowed
```

**解决方法：**
1. 增加堆大小：修改 `rtconfig.h` 中的 `RT_HEAP_SIZE`
2. 减小 WIFI 任务栈：修改 Kconfig 中的 `GD32VW55X_WIFI_TASK_STACK_SIZE`
3. 优化编译选项：使用 `-Os` 而不是 `-O0`

### 问题 5: 标准外设库头文件找不到

**错误信息：**
```
fatal error: gd32vw55x.h: No such file or directory
```

**解决方法：**
检查 SConscript 是否正确引用了 gd32-riscv-series-latest 包：
```python
gd32_peripheral_path = os.path.join(bsp_packages_dir,
    'gd32-riscv-series-latest', 'GD32VW55x', 'GD32VW55x_standard_peripheral')
```

---

## 🧪 编译后功能测试

### 1. 烧录固件

使用 OpenOCD 或 GDLink 烧录：

```bash
# 使用 OpenOCD
openocd -f openocd_gdlink.cfg -c "program rtthread.elf verify reset exit"
```

### 2. 连接串口

使用串口工具（PuTTY、MobaXterm 等）：
- 波特率: 115200
- 数据位: 8
- 停止位: 1
- 校验: 无

### 3. 验证系统启动

应该看到 RT-Thread 启动信息：

```
 \ | /
- RT -     Thread Operating System
 / | \     x.x.x build Dec 25 2025
 2006 - 2024 Copyright by RT-Thread team
msh >
```

### 4. 检查 WIFI 模块

```bash
# 列出可用命令
msh > help

# 查看 WIFI 相关命令
msh > wifi_sta_sample
msh > wifi_mem_info
```

### 5. 测试 WIFI 连接（如果示例代码已编译）

修改 `samples/wifi_sta_sample.c` 中的 SSID 和密码，重新编译后：

```bash
msh > wifi_sta_sample
```

观察输出：
```
[I/wifi.sample.sta] WIFI Station Sample Start
[I/wifi.sample.sta] WIFI initialized
[I/wifi.sample.sta] Start scanning...
[I/wifi.sample.sta] Connecting to AP: YourSSID
[I/wifi.sample.sta] WIFI Connected!
[I/wifi.sample.sta]   SSID: YourSSID
[I/wifi.sample.sta]   IP  : 192.168.1.100
```

---

## 📊 性能和内存检查

### 1. 查看内存使用

```bash
msh > free
msh > list_thread
```

### 2. 检查 WIFI 任务

```bash
msh > list_thread

thread   pri  status      sp     stack size max used left tick  error
-------- ---  ------- ---------- ----------  ------  ---------- ---
wifi_mgr  10  suspend 0x00000xxx 0x00001000    xx%   0x00000xxx 000
```

### 3. 监控网络状态

```bash
msh > ifconfig
msh > ping 192.168.1.1
```

---

## 🎯 下一步优化

### 1. 完善移植层

实现 `port/` 目录下的适配函数：
- `wifi_netif_port.c` - netdev 对接
- `wifi_mem_port.c` - 内存管理验证

### 2. 测试完整功能

- Station 模式连接测试
- DHCP 获取 IP 测试
- TCP/UDP 通信测试
- WPA/WPA2 安全测试

### 3. 性能调优

- 调整任务优先级
- 优化内存使用
- 配置功耗管理

---

## 📝 快速命令参考

```bash
# 配置
scons --menuconfig                    # 打开配置菜单

# 编译
scons -c                             # 清理
scons -j8                            # 编译（8线程）
scons -j8 --verbose                  # 详细编译信息

# 调试
scons --verbose > build.log 2>&1     # 保存编译日志
```

---

## 🔗 相关文档

- [README.md](README.md) - 软件包使用说明
- [PORTING.md](PORTING.md) - 移植步骤详细说明
- [RT-Thread 文档](https://www.rt-thread.org/document/site/)
- [SCons 用户手册](https://scons.org/documentation.html)

---

如有问题，请检查：
1. ✓ 所有 SDK 文件已正确复制
2. ✓ Kconfig 配置已启用 WIFI 包
3. ✓ 工具链路径正确配置
4. ✓ 预编译库与工具链兼容
5. ✓ 内存配置足够大（建议 >= 128KB 堆）
