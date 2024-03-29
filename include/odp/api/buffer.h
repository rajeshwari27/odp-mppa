/* Copyright (c) 2013, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */


/**
 * @file
 *
 * ODP buffer descriptor
 */

#ifndef ODP_API_BUFFER_H_
#define ODP_API_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup odp_buffer ODP BUFFER
 *  Operations on a buffer.
 *  @{
 */


/**
 * @typedef odp_buffer_t
 * ODP buffer
 */

/**
 * @def ODP_BUFFER_INVALID
 * Invalid buffer
 */

/**
 * @typedef odp_buffer_seg_t
 * ODP buffer segment
 */

/**
 * @def ODP_SEGMENT_INVALID
 * Invalid segment
 */

/**
 * Get buffer handle from event
 *
 * Converts an ODP_EVENT_BUFFER type event to a buffer.
 *
 * @param ev   Event handle
 *
 * @return Buffer handle
 *
 * @see odp_event_type()
 */
odp_buffer_t odp_buffer_from_event(odp_event_t ev);

/**
 * Convert buffer handle to event
 *
 * @param buf  Buffer handle
 *
 * @return Event handle
 */
odp_event_t odp_buffer_to_event(odp_buffer_t buf);

/**
 * Buffer start address
 *
 * @param buf      Buffer handle
 *
 * @return Buffer start address
 */
void *odp_buffer_addr(odp_buffer_t buf);

/**
 * Buffer maximum data size
 *
 * @param buf      Buffer handle
 *
 * @return Buffer maximum data size
 */
uint32_t odp_buffer_size(odp_buffer_t buf);

/**
 * Tests if buffer is valid
 *
 * @param buf      Buffer handle
 *
 * @retval 1 Buffer handle represents a valid buffer.
 * @retval 0 Buffer handle does not represent a valid buffer.
 */
int odp_buffer_is_valid(odp_buffer_t buf);

/**
 * Buffer pool of the buffer
 *
 * @param buf       Buffer handle
 *
 * @return Handle of buffer pool buffer belongs to
 */
odp_pool_t odp_buffer_pool(odp_buffer_t buf);

/**
 * Buffer alloc
 *
 * The validity of a buffer can be cheked at any time with odp_buffer_is_valid()
 * @param pool      Pool handle
 *
 * @return Handle of allocated buffer
 * @retval ODP_BUFFER_INVALID  Buffer could not be allocated
 */
odp_buffer_t odp_buffer_alloc(odp_pool_t pool);

/**
 * Buffer free
 *
 * @param buf       Buffer handle
 *
 */
void odp_buffer_free(odp_buffer_t buf);

/**
 * Print buffer metadata to STDOUT
 *
 * @param buf      Buffer handle
 *
 */
void odp_buffer_print(odp_buffer_t buf);

/**
 * Get printable value for an odp_buffer_t
 *
 * @param hdl  odp_buffer_t handle to be printed
 * @return     uint64_t value that can be used to print/display this
 *             handle
 *
 * @note This routine is intended to be used for diagnostic purposes
 * to enable applications to generate a printable value that represents
 * an odp_buffer_t handle.
 */
uint64_t odp_buffer_to_u64(odp_buffer_t hdl);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
