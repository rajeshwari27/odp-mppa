/* Copyright (c) 2015, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */


/**
 * @file
 *
 * ODP atomic operations
 */

#ifndef ODP_ATOMIC_TYPES_H_
#define ODP_ATOMIC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <odp/align.h>
#include <HAL/hal/hal.h>

/**
 * @internal
 * Atomic 64-bit unsigned integer
 */
struct odp_atomic_u64_s {
	uint64_t v; /**< Actual storage for the atomic variable */
	/* Some architectures do not support lock-free operations on 64-bit
	 * data types. We use a spin lock to ensure atomicity. */
	uint64_t lock; /**< Spin lock (if needed) used to ensure atomic access */
} ODP_ALIGNED(sizeof(uint64_t)); /* Enforce alignement! */;

/**
 * @internal
 * Atomic 32-bit unsigned integer
 */
struct odp_atomic_u32_s {
	uint32_t v; /**< Actual storage for the atomic variable */
	uint64_t lock; /**< Spin lock (if needed) used to ensure atomic access */
} ODP_ALIGNED(sizeof(uint32_t)); /* Enforce alignement! */;

/**
 * @internal
 * Helper macro for lock-based atomic operations on 64-bit integers
 * @param[in,out] atom Pointer to the 64-bit atomic variable
 * @param expr Expression used update the variable.
 * @return The old value of the variable.
 */
#define ATOMIC_OP(atom, expr)													\
	({																			\
	uint64_t old_val;															\
	/* Loop while lock is already taken, stop when lock becomes clear */		\
	while (!__k1_atomic_test_and_clear(&atom->lock))							\
		__k1_cpu_backoff(10);													\
	__k1_dcache_invalidate_line((__k1_uintptr_t)&atom->v);						\
	old_val = (atom)->v;														\
	(expr); /* Perform whatever update is desired */							\
	__k1_wmb();																	\
	__builtin_k1_swu(&(atom)->lock, 0x1ULL);									\
	old_val; /* Return old value */												\
})

/** @addtogroup odp_synchronizers
 *  @{
 */

typedef struct odp_atomic_u64_s odp_atomic_u64_t;

typedef struct odp_atomic_u32_s odp_atomic_u32_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif