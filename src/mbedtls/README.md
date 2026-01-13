# mbedtls 本地编译配置

## 目录结构

```
src/mbedtls/
├── mbedtls/            # mbedtls 源码（从 GD32 SDK 拷贝）
│   ├── include/        # 头文件
│   │   └── mbedtls/
│   │       ├── mbedtls_config.h
│   │       └── ...
│   └── library/        # 源文件
│       ├── aes.c
│       ├── ecdh.c
│       └── ...
└── README.md          # 本文件
```

## SConscript 配置说明

SConscript 已配置为：

1. **头文件路径**: 自动添加 `src/mbedtls/mbedtls/include`
2. **源文件编译**: 自动编译 `src/mbedtls/mbedtls/library/*.c`
3. **排除文件**:
   - `net_sockets.c` - 网络套接字（使用 LWIP）
   - `timing.c` - 时序函数（需要平台适配）
4. **预编译库**: 当检测到本地 mbedtls 源码时，自动排除预编译的 `libwpas.a`

## 配置文件要求

mbedtls 需要配置文件 `mbedtls_config.h`，可以从以下位置拷贝：

```
GD32VW55x_RELEASE_V1.0.3f/MSDK/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h
```

拷贝到：
```
packages/gd32vw55x-wifi/src/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h
```

## 编译选项

mbedtls 使用的编译选项：
- `-mabi=ilp32 -march=rv32imac` - 软浮点 RISC-V ABI（匹配预编译库）
- `MBEDTLS_CONFIG_FILE=<mbedtls/mbedtls_config.h>` - 配置文件宏定义

## 注意事项

### 1. 配置文件定制
根据 WiFi 功能需求，可能需要在 `mbedtls_config.h` 中启用/禁用特定功能：
- WPA2: AES, SHA256, HMAC
- WPA3/SAE: ECC (ECDH, ECDSA), SHA256, HMAC
- 根据实际内存限制调整缓冲区大小

### 2. 平台适配
某些 mbedtls 功能需要平台适配：
- `timing.c` - 时序函数（已排除，可选实现）
- 随机数生成器 - 需要连接到硬件 TRNG
- 内存分配 - 可使用 RT-Thread 的内存管理

### 3. 依赖关系
- mbedtls 编译后，wpas 相关的加密功能将使用本地编译版本
- 预编译的 `libwpas.a` 将被排除，避免符号冲突

## 从 GD32 SDK 拷贝 mbedtls

### 完整拷贝（推荐）
```bash
# 从 GD32 SDK 拷贝整个 mbedtls 目录
cp -r GD32VW55x_RELEASE_V1.0.3f/MSDK/mbedtls/mbedtls \
      packages/gd32vw55x-wifi/src/mbedtls/
```

### 最小拷贝（仅需要的文件）
如果想减小包大小，只拷贝必需的部分：
```bash
# 拷贝头文件
cp -r GD32VW55x_RELEASE_V1.0.3f/MSDK/mbedtls/mbedtls/include \
      packages/gd32vw55x-wifi/src/mbedtls/mbedtls/

# 拷贝库文件
cp -r GD32VW55x_RELEASE_V1.0.3f/MSDK/mbedtls/mbedtls/library \
      packages/gd32vw55x-wifi/src/mbedtls/mbedtls/
```

## 验证编译

编译成功后，应该看到类似输出：
```
CC build\packages\gd32vw55x-wifi\src\mbedtls\mbedtls\library\aes.o
CC build\packages\gd32vw55x-wifi\src\mbedtls\mbedtls\library\ecdh.o
CC build\packages\gd32vw55x-wifi\src\mbedtls\mbedtls\library\sha256.o
...
```

并且链接时不再出现 `undefined reference to 'mbedtls_*'` 错误。
