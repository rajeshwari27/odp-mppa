/* Copyright (c) 2014, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/**
 * @file
 *
 * ODP atomic types and operations, semantically a subset of C11 atomics.
 * Reuse the 32-bit and 64-bit type definitions from odp_atomic.h. Introduces
 * new atomic pointer and flag types.
 * Atomic functions must be used to operate on atomic variables!
 */

#ifndef ODP_ATOMIC_INTERNAL_H_
#define ODP_ATOMIC_INTERNAL_H_

#include <odp/std_types.h>
#include <odp/align.h>
#include <odp/hints.h>
#include <odp/atomic.h>
#include <HAL/hal/hal.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup odp_synchronizers
 *  Atomic operations.
 *  @{
 */

/**
 * Pointer atomic type
 */
typedef struct {
	void *v; /**< Actual storage for the atomic variable */
} _odp_atomic_ptr_t
ODP_ALIGNED(sizeof(void *)); /* Enforce alignement! */

/**
 * Atomic flag (boolean) type
 * @Note this is not the same as a plain boolean type.
 * _odp_atomic_flag_t is guaranteed to be able to operate on atomically.
 */
typedef uint64_t _odp_atomic_flag_t;

/**
 * Memory orderings supported by ODP.
 */
typedef enum {
/** Relaxed memory ordering, no ordering of other accesses enforced.
 * Atomic operations with relaxed memory ordering cannot be used for
 * synchronization */
	_ODP_MEMMODEL_RLX = __ATOMIC_RELAXED,
/** Acquire memory ordering, synchronize with release stores from another
 * thread (later accesses cannot move before acquire operation).
 * Use acquire and release memory ordering for Release Consistency */
	_ODP_MEMMODEL_ACQ = __ATOMIC_ACQUIRE,
/** Release memory ordering, synchronize with acquire loads from another
 * thread (earlier accesses cannot move after release operation).
 * Use acquire and release memory ordering for Release Consistency */
	_ODP_MEMMODEL_RLS = __ATOMIC_RELEASE,
/** Acquire&release memory ordering, synchronize with acquire loads and release
 * stores in another (one other) thread */
	_ODP_MEMMODEL_ACQ_RLS = __ATOMIC_ACQ_REL,
/** Sequential consistent memory ordering, synchronize with acquire loads and
 * release stores in all threads */
	_ODP_MEMMODEL_SC = __ATOMIC_SEQ_CST
} _odp_memmodel_t;

/**
 * Insert a full memory barrier (fence) in the compiler and instruction
 * sequence.
 */
#define _ODP_FULL_BARRIER() __k1_mb()


/*****************************************************************************
 * Operations on flag atomics
 * _odp_atomic_flag_init - no return value
 * _odp_atomic_flag_load - return current value
 * _odp_atomic_flag_tas - return old value
 * _odp_atomic_flag_clear - no return value
 *
 * Flag atomics use Release Consistency memory consistency model, acquire
 * semantics for TAS and release semantics for clear.
 *****************************************************************************/

/**
 * Initialize a flag atomic variable
 *
 * @param[out] flag Pointer to a flag atomic variable
 * @param val The initial value of the variable
 */
static inline void _odp_atomic_flag_init(_odp_atomic_flag_t *flag,
		odp_bool_t val)
{
	__builtin_k1_wpurge();
	__builtin_k1_swu(flag, val);
	__builtin_k1_fence();
}

/**
 * Load atomic flag variable
 * @Note Operation has relaxed semantics.
 *
 * @param flag Pointer to a flag atomic variable
 * @return The current value of the variable
 */
static inline int _odp_atomic_flag_load(_odp_atomic_flag_t *flag)
{
	return __builtin_k1_lwu(flag);
}

/**
 * Test-and-set of atomic flag variable
 * @Note Operation has acquire semantics. It pairs with a later
 * release operation.
 *
 * @param[in,out] flag Pointer to a flag atomic variable
 *
 * @retval 1 if the flag was already true - lock not taken
 * @retval 0 if the flag was false and is now set to true - lock taken
 */
static inline int _odp_atomic_flag_tas(_odp_atomic_flag_t *flag)
{
	return !__k1_atomic_test_and_clear(flag);
}

/**
 * Clear atomic flag variable
 * The flag variable is cleared (set to false).
 * @Note Operation has release semantics. It pairs with an earlier
 * acquire operation or a later load operation.
 *
 * @param[out] flag Pointer to a flag atomic variable
 */
static inline void _odp_atomic_flag_clear(_odp_atomic_flag_t *flag)
{
	__builtin_k1_swu(flag, 0x1ULL);
}

/*****************************************************************************
 * Operations on 64-bit atomics
 * _odp_atomic_u64_load_mm - return current value
 * _odp_atomic_u64_store_mm - no return value
 * _odp_atomic_u64_xchg_mm - return old value
 * _odp_atomic_u64_cmp_xchg_strong_mm - return bool
 * _odp_atomic_u64_fetch_add_mm - return old value
 * _odp_atomic_u64_add_mm - no return value
 * _odp_atomic_u64_fetch_sub_mm - return old value
 * _odp_atomic_u64_sub_mm - no return value
 *****************************************************************************/

/**
 * @internal
 * Helper macro for lock-based atomic operations on 64-bit integers
 * @param[in,out] atom Pointer to the 64-bit atomic variable
 * @param expr Expression used update the variable.
 * @param mm Memory order to use.
 * @return The old value of the variable.
 */
#define ATOMIC_OP_MM(atom, expr, mm) \
({ \
	 uint64_t old_val; \
	 /* Loop while lock is already taken, stop when lock becomes clear */ \
	 while (_odp_atomic_flag_tas(&(atom)->lock))						  \
		 (void)mm;														  \
	 __k1_dcache_invalidate_line((__k1_uintptr_t)&atom->v);				  \
	 old_val = (atom)->v;												  \
	 (expr); /* Perform whatever update is desired */					  \
	 __k1_wmb();														  \
	_odp_atomic_flag_clear(&(atom)->lock);								  \
	 old_val; /* Return old value */									  \
})

/*****************************************************************************
 * Operations on 32-bit atomics
 * _odp_atomic_u32_load_mm - return current value
 * _odp_atomic_u32_store_mm - no return value
 * _odp_atomic_u32_xchg_mm - return old value
 * _odp_atomic_u32_cmp_xchg_strong_mm - return bool
 * _odp_atomic_u32_fetch_add_mm - return old value
 * _odp_atomic_u32_add_mm - no return value
 * _odp_atomic_u32_fetch_sub_mm - return old value
 * _odp_atomic_u32_sub_mm - no return value
 *****************************************************************************/

/**
 * Atomic load of 32-bit atomic variable
 *
 * @param atom Pointer to a 32-bit atomic variable
 * @param mmodel Memory ordering associated with the load operation
 *
 * @return Value of the variable
 */
static inline uint32_t _odp_atomic_u32_load_mm(const odp_atomic_u32_t *atom,
		_odp_memmodel_t mmodel ODP_UNUSED)
{
	return __builtin_k1_lwu((void*)&atom->v);
}

/**
 * Atomic store to 32-bit atomic variable
 *
 * @param[out] atom Pointer to a 32-bit atomic variable
 * @param val    Value to store in the atomic variable
 * @param mmodel Memory order associated with the store operation
 */
static inline void _odp_atomic_u32_store_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel ODP_UNUSED)
{
	__builtin_k1_swu(&atom->v, val);
}

/**
 * Atomic exchange (swap) of 32-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param val    New value to store in the atomic variable
 * @param mmodel Memory order associated with the exchange operation
 *
 * @return Old value of the variable
 */
static inline uint32_t _odp_atomic_u32_xchg_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel)

{
	uint32_t orgval, oldval;
	do {
		orgval = _odp_atomic_u32_load_mm(atom, mmodel);
		oldval = __builtin_k1_cws(&atom->v, val, orgval);
	} while(orgval != oldval);
	return oldval;
}

/**
 * Atomic compare and exchange (swap) of 32-bit atomic variable
 * "Strong" semantics, will not fail spuriously.
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param[in,out] exp Pointer to expected value (updated on failure)
 * @param val   New value to write
 * @param success Memory order associated with a successful compare-and-swap
 * operation
 * @param failure Memory order associated with a failed compare-and-swap
 * operation
 *
 * @retval 1 exchange successul
 * @retval 0 exchange failed and '*exp' updated with current value
 */
static inline int _odp_atomic_u32_cmp_xchg_strong_mm(
		odp_atomic_u32_t *atom,
		uint32_t *exp,
		uint32_t val,
		_odp_memmodel_t success ODP_UNUSED,
		_odp_memmodel_t failure ODP_UNUSED)
{
	uint32_t orgval, oldval;
	orgval = __builtin_k1_lwu(exp);
	oldval = __builtin_k1_cws(&atom->v, val, orgval);

	if(orgval == oldval){
		return 1;
	}

	*exp = orgval;
	return 0;
}

/**
 * Atomic fetch and add of 32-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param val Value to add to the atomic variable
 * @param mmodel Memory order associated with the add operation
 *
 * @return Value of the atomic variable before the addition
 */
static inline uint32_t _odp_atomic_u32_fetch_add_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel)
{
	return ATOMIC_OP_MM(atom, atom->v += val, mmodel);
}

/**
 * Atomic add of 32-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param val Value to add to the atomic variable
 * @param mmodel Memory order associated with the add operation
 */
static inline void _odp_atomic_u32_add_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel)

{
	(void)ATOMIC_OP_MM(atom, atom->v += val, mmodel);
}

/**
 * Atomic fetch and subtract of 32-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param val Value to subtract from the atomic variable
 * @param mmodel Memory order associated with the subtract operation
 *
 * @return Value of the atomic variable before the subtraction
 */
static inline uint32_t _odp_atomic_u32_fetch_sub_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel)
{
	return ATOMIC_OP_MM(atom, atom->v -= val, mmodel);
}

/**
 * Atomic subtract of 32-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 32-bit atomic variable
 * @param val Value to subtract from the atomic variable
 * @param mmodel Memory order associated with the subtract operation
 */
static inline void _odp_atomic_u32_sub_mm(odp_atomic_u32_t *atom,
		uint32_t val,
		_odp_memmodel_t mmodel)

{
	(void)ATOMIC_OP_MM(atom, atom->v -= val, mmodel);
}

/**
 * Atomic load of 64-bit atomic variable
 *
 * @param atom Pointer to a 64-bit atomic variable
 * @param mmodel Memory order associated with the load operation
 *
 * @return Value of the variable
 */
static inline uint64_t _odp_atomic_u64_load_mm(odp_atomic_u64_t *atom,
		_odp_memmodel_t mmodel)
{
	return ATOMIC_OP_MM(atom, (void)0, mmodel);
}

/**
 * Atomic store to 64-bit atomic variable
 *
 * @param[out] atom Pointer to a 64-bit atomic variable
 * @param val  Value to write to the atomic variable
 * @param mmodel Memory order associated with the store operation
 */
static inline void _odp_atomic_u64_store_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)
{
	(void)ATOMIC_OP_MM(atom, atom->v = val, mmodel);
}

/**
 * Atomic exchange (swap) of 64-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param val   New value to write to the atomic variable
 * @param mmodel Memory order associated with the exchange operation
 *
 * @return Old value of variable
 */
static inline uint64_t _odp_atomic_u64_xchg_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)

{
	return ATOMIC_OP_MM(atom, atom->v = val, mmodel);
}

/**
 * Atomic compare and exchange (swap) of 64-bit atomic variable
 * "Strong" semantics, will not fail spuriously.
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param[in,out] exp Pointer to expected value (updated on failure)
 * @param val   New value to write
 * @param success Memory order associated with a successful compare-and-swap
 * operation
 * @param failure Memory order associated with a failed compare-and-swap
 * operation
 *
 * @retval 1 exchange successful
 * @retval 0 exchange failed and '*exp' updated with current value
 */
static inline int _odp_atomic_u64_cmp_xchg_strong_mm(odp_atomic_u64_t *atom,
		uint64_t *exp,
		uint64_t val,
		_odp_memmodel_t success ODP_UNUSED,
		_odp_memmodel_t failure ODP_UNUSED)
{
	/* Possibly we are a bit pessimistic with the memory models */
	odp_bool_t ret_succ;
	/* Loop while lock is already taken, stop when lock becomes clear */
	while (_odp_atomic_flag_tas(&(atom)->lock))
		(void)0;
	if (atom->v == *exp) {
		atom->v = val;
		ret_succ = 1;
	} else {
		*exp = atom->v;
		ret_succ = 0;
	}
	_odp_atomic_flag_clear(&(atom)->lock);
	return ret_succ;

}

/**
 * Atomic fetch and add of 64-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param val   Value to add to the atomic variable
 * @param mmodel Memory order associated with the add operation
 *
 * @return Value of the atomic variable before the addition
 */
static inline uint64_t _odp_atomic_u64_fetch_add_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)
{
	return ATOMIC_OP_MM(atom, atom->v += val, mmodel);
}

/**
 * Atomic add of 64-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param val   Value to add to the atomic variable
 * @param mmodel Memory order associated with the add operation.
 */
static inline void _odp_atomic_u64_add_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)

{
	(void)ATOMIC_OP_MM(atom, atom->v += val, mmodel);
}

/**
 * Atomic fetch and subtract of 64-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param val   Value to subtract from the atomic variable
 * @param mmodel Memory order associated with the subtract operation
 *
 * @return Value of the atomic variable before the subtraction
 */
static inline uint64_t _odp_atomic_u64_fetch_sub_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)
{
	return ATOMIC_OP_MM(atom, atom->v -= val, mmodel);
}

/**
 * Atomic subtract of 64-bit atomic variable
 *
 * @param[in,out] atom Pointer to a 64-bit atomic variable
 * @param val   Value to subtract from the atomic variable
 * @param mmodel Memory order associated with the subtract operation
 */
static inline void _odp_atomic_u64_sub_mm(odp_atomic_u64_t *atom,
		uint64_t val,
		_odp_memmodel_t mmodel)

{
	(void)ATOMIC_OP_MM(atom, atom->v -= val, mmodel);
}

/*****************************************************************************
 * Operations on pointer atomics
 * _odp_atomic_ptr_init - no return value
 * _odp_atomic_ptr_load - return current value
 * _odp_atomic_ptr_store - no return value
 * _odp_atomic_ptr_xchg - return old value
 *****************************************************************************/

/**
 * Initialization of pointer atomic variable
 *
 * @param[out] atom Pointer to a pointer atomic variable
 * @param val   Value to initialize the variable with
 */
static inline void _odp_atomic_ptr_init(_odp_atomic_ptr_t *atom, void *val)
{
	_odp_atomic_u32_store_mm((odp_atomic_u32_t*)atom, (uint32_t)val, __ATOMIC_RELAXED);
}

/**
 * Atomic load of pointer atomic variable
 *
 * @param atom Pointer to a pointer atomic variable
 * @param mmodel Memory order associated with the load operation
 *
 * @return Value of the variable
 */
static inline void *_odp_atomic_ptr_load(const _odp_atomic_ptr_t *atom,
		_odp_memmodel_t mmodel)
{
	return (void*)_odp_atomic_u32_load_mm((odp_atomic_u32_t*)atom, mmodel);
}

/**
 * Atomic store to pointer atomic variable
 *
 * @param[out] atom Pointer to a pointer atomic variable
 * @param val  Value to write to the atomic variable
 * @param mmodel Memory order associated with the store operation
 */
static inline void _odp_atomic_ptr_store(_odp_atomic_ptr_t *atom,
		void *val,
		_odp_memmodel_t mmodel)
{
	_odp_atomic_u32_store_mm((odp_atomic_u32_t*)atom, (uint32_t)val, mmodel);
}

/**
 * Atomic exchange (swap) of pointer atomic variable
 *
 * @param[in,out] atom Pointer to a pointer atomic variable
 * @param val   New value to write
 * @param mmodel Memory order associated with the exchange operation
 *
 * @return Old value of variable
 */
static inline void *_odp_atomic_ptr_xchg(_odp_atomic_ptr_t *atom,
		void *val,
		_odp_memmodel_t mmodel)
{
	return (void*)_odp_atomic_u32_xchg_mm((odp_atomic_u32_t*)atom, (uint32_t)val, mmodel);
}

/**
 * Atomic compare and exchange (swap) of pointer atomic variable
 * "Strong" semantics, will not fail spuriously.
 *
 * @param[in,out] atom Pointer to a pointer atomic variable
 * @param[in,out] exp Pointer to expected value (updated on failure)
 * @param val   New value to write
 * @param success Memory order associated with a successful compare-and-swap
 * operation
 * @param failure Memory order associated with a failed compare-and-swap
 * operation
 *
 * @retval 1 exchange successful
 * @retval 0 exchange failed and '*exp' updated with current value
 */
static inline int _odp_atomic_ptr_cmp_xchg_strong(
		_odp_atomic_ptr_t *atom,
		void **exp,
		void *val,
		_odp_memmodel_t success,
		_odp_memmodel_t failure)
{
	return _odp_atomic_u32_cmp_xchg_strong_mm((odp_atomic_u32_t*)atom, (uint32_t*) exp, (uint32_t)val, success, failure);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif