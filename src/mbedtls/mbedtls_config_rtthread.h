/**
 * \file mbedtls_config_rtthread.h
 *
 * \brief RT-Thread wrapper for mbedtls configuration
 *
 * This file provides a simpler configuration wrapper for RT-Thread,
 * avoiding circular dependencies during compilation.
 */

#ifndef MBEDTLS_CONFIG_RTTHREAD_H
#define MBEDTLS_CONFIG_RTTHREAD_H

/* Include the original GD32 mbedtls configuration */
#include "mbedtls/mbedtls_config.h"

#endif /* MBEDTLS_CONFIG_RTTHREAD_H */
