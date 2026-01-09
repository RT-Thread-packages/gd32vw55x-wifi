# GD32VW55x WiFi 包移植问题说明

## 已修复的问题

### 1. crypto 函数多重定义
**问题**: `wpas_sae_crypto_mbedtls.c` 被重复编译
- 该文件被 `wifi_wpa.c` 通过 `#include` 包含
- SConscript 又独立编译了一次

**解决**: 在 SConscript 中排除此文件

### 2. 浮点 ABI 不匹配
**问题**: 预编译库使用软浮点，项目可能使用硬浮点
**解决**: 添加 `-mabi=ilp32 -march=rv32imac` 编译选项

## 需要解决的问题

### 1. 缺少 mbedtls 库链接
**错误**: 大量 `mbedtls_*` 函数未定义
**位置**: GD32VW55x_RELEASE_V1.0.3f/MSDK/mbedtls/mbedtls-2.17.0-ssl/

**解决方案**:
```python
# 在 SConscript 中添加
mbedtls_lib_path = os.path.join(gd32_sdk_root, 'MSDK', 'mbedtls', 'mbedtls-2.17.0-ssl', 'library')
if os.path.exists(mbedtls_lib_path):
    LIBPATH += [mbedtls_lib_path]
    LIBS += ['mbedtls', 'mbedcrypto', 'mbedx509']
```

或者需要编译 mbedtls 源码并链接。

### 2. 缺少 GD32 HAL 外设驱动
**未定义引用**:
- **TRNG** (真随机数生成器): `trng_flag_get`, `trng_enable`, `trng_get_true_random_data`
- **HAU** (Hash 加速单元): `hau_hash_sha_256`, `hau_hmac_sha_256`, `hau_hmac_sha_1`, `hau_hmac_md5`
- **CAU** (密码加速单元): `cau_deinit`, `cau_struct_para_init`, `cau_aes_ecb`
- **PMU** (电源管理): `pmu_wifi_power_enable`, `pmu_wifi_sram_control`, `pmu_flag_get`

**解决方案**:
需要添加 GD32VW55x 标准外设库的相应驱动文件到项目中，或者在 ROM 符号表中找到这些函数。

### 3. 缺少 DSP 库
**未定义引用**: `riscv_sin_cos_f32`, `riscv_cfft_*`

**解决方案**: 链接 NMSIS DSP 库或 GD32 提供的 DSP 库。

### 4. 缺少网络层实现
**未定义引用**:
- `net_init`, `net_if_*`, `net_dhcp_*`, `net_dhcpd_*`
- `net_buf_*`, `net_lpbk_*`, `net_l2_send`
- `dhcpd_delete_ipaddr_by_macaddr`

**位置**: 这些函数应该在 GD32 SDK 的网络协议栈中实现
- 可能在 `GD32VW55x_RELEASE_V1.0.3f/MSDK/lwip/` 或类似目录

**解决方案**:
1. 找到 GD32 SDK 中的网络层实现源码
2. 添加到 SConscript 编译
3. 或者实现 RT-Thread 到 GD32 网络层的适配

### 5. FreeRTOS 临界区函数
**未定义引用**: `vPortInCritical`, `vPortEnterCritical`, `vPortExitCritical`

**位置**: `wrapper_rtthread.c` 中调用

**解决方案**: 需要修改 wrapper_rtthread.c，将这些 FreeRTOS 函数替换为 RT-Thread 等价实现：
```c
// 替换
rt_base_t rt_hw_interrupt_disable(void);  // 代替 vPortEnterCritical
void rt_hw_interrupt_enable(rt_base_t level);  // 代替 vPortExitCritical
```

### 6. RT-Thread memheap
**未定义引用**: `rt_memheap_init`

**解决方案**: 在 RT-Thread 配置中启用 memheap 组件，或修改代码使用标准内存管理。

### 7. sys_mutex_* 函数链接问题
**错误**: libwifi.a 引用 `sys_mutex_init`, `sys_mutex_get`, `sys_mutex_put`

**问题**: 虽然在 wrapper_rtthread.c 中实现了，但链接器找不到

**解决方案**:
1. 确保链接顺序：用户代码库应在预编译库之前
2. 可能需要使用 `--whole-archive` 选项强制链接所有符号

### 8. 浮点软件库函数
**未定义引用**: `__extendsfdf2`, `__muldf3`, `__truncdfsf2`, `__floatsidf`, `__fixdfsi`, `__adddf3`, `__divdf3`, `__subdf3`

**原因**: 软浮点模式下需要软件实现的浮点运算

**解决方案**:
- 链接器应该自动链接 libgcc，检查链接器设置
- 或添加 `-lgcc` 到链接选项

### 9. rom_symbol_init 重复定义
**问题**: init_rom.c 和 init_rom_symbol.c 都定义了 `rom_symbol_init`

**解决方案**: 检查是否需要两个文件，可能只需要保留一个。

## 推荐的移植步骤

1. **首先**: 确认并添加 mbedtls 库链接
2. **其次**: 添加 GD32 HAL 外设驱动（TRNG, HAU, CAU, PMU）
3. **然后**: 实现或适配网络层接口
4. **修改**: wrapper_rtthread.c 中的 FreeRTOS 函数调用
5. **最后**: 处理其他辅助库依赖

## 注意事项

### 浮点 ABI 兼容性
当前设置强制使用软浮点 (`-mabi=ilp32 -march=rv32imac`)，这可能与 RT-Thread 主项目的硬浮点设置冲突。

**建议**:
- 选项 1: 将整个项目改为软浮点编译
- 选项 2: 联系 GD32 获取硬浮点版本的预编译库
- 选项 3: 获取 WiFi 库源码重新编译

### 预编译库的局限性
使用预编译库 (libwifi.a, libwpas.a, librf.a) 的问题：
- 必须匹配编译器版本和ABI
- 无法修改内部实现
- 调试困难

**建议**: 如果可能，尝试获取这些库的源代码进行编译。
