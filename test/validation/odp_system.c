/* Copyright (c) 2014, Linaro Limited
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "odp.h"
#include "odp_cunit_common.h"

static void test_odp_sys_core_count(void)
{
	int cores;

	cores = odp_sys_core_count();
	CU_ASSERT(0 < cores);
}

static void test_odp_sys_cache_line_size(void)
{
	uint64_t cache_size;

	cache_size = odp_sys_cache_line_size();
	CU_ASSERT(0 < cache_size);
	CU_ASSERT(ODP_CACHE_LINE_SIZE == cache_size);
}

static void test_odp_sys_cpu_model_str(void)
{
	char model[128];

	snprintf(model, 128, "%s", odp_sys_cpu_model_str());
	CU_ASSERT(strlen(model) > 0);
	CU_ASSERT(strlen(model) < 127);
}

static void test_odp_sys_page_size(void)
{
	uint64_t page;

	page = odp_sys_page_size();
	CU_ASSERT(0 < page);
	CU_ASSERT(ODP_PAGE_SIZE == page);
}

static void test_odp_sys_huge_page_size(void)
{
	uint64_t page;

	page = odp_sys_huge_page_size();
	CU_ASSERT(0 < page);
}

static void test_odp_sys_cpu_hz(void)
{
	uint64_t hz;

	hz = odp_sys_cpu_hz();
	CU_ASSERT(0 < hz);
}

CU_TestInfo test_odp_system[] = {
	{"odp_sys_core_count",  test_odp_sys_core_count},
	{"odp_sys_cache_line_size",  test_odp_sys_cache_line_size},
	{"odp_sys_cpu_model_str",  test_odp_sys_cpu_model_str},
	{"odp_sys_page_size",  test_odp_sys_page_size},
	{"odp_sys_huge_page_size",  test_odp_sys_huge_page_size},
	{"odp_sys_cpu_hz",  test_odp_sys_cpu_hz},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo odp_testsuites[] = {
		{"System Info", NULL, NULL, NULL, NULL,
		 test_odp_system},
		 CU_SUITE_INFO_NULL,
};
