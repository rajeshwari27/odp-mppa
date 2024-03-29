/* Copyright (c) 2015, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */


/**
 * @file
 *
 * ODP CPU masks and enumeration
 */

#ifndef ODP_CPUMASK_TYPES_H_
#define ODP_CPUMASK_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup odp_compiler_optim
 *  @{
 */

#include <odp/std_types.h>

/**
 * Minimum size of output buffer for odp_cpumask_to_str()
 */
#define ODP_CPUMASK_STR_SIZE ((ODP_CONFIG_MAX_THREADS + 3) / 4 + 3)

/**
 * CPU mask
 *
 * Don't access directly, use access functions.
 */
typedef struct odp_cpumask_t {
	cpu_set_t set; /**< @private Mask*/
} odp_cpumask_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
