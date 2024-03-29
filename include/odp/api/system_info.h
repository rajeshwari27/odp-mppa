/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */


/**
 * @file
 *
 * ODP system information
 */

#ifndef ODP_API_SYSTEM_INFO_H_
#define ODP_API_SYSTEM_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup odp_ver_abt_log_dbg
 *  @{
 */

/**
 * CPU frequency in Hz
 *
 * @return CPU frequency in Hz
 */
uint64_t odp_sys_cpu_hz(void);

/**
 * Huge page size in bytes
 *
 * @return Huge page size in bytes
 */
uint64_t odp_sys_huge_page_size(void);

/**
 * Page size in bytes
 *
 * @return Page size in bytes
 */
uint64_t odp_sys_page_size(void);

/**
 * CPU model name
 *
 * @return Pointer to CPU model name string
 */
const char *odp_sys_cpu_model_str(void);

/**
 * Cache line size in bytes
 *
 * @return CPU cache line size in bytes
 */
int odp_sys_cache_line_size(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
