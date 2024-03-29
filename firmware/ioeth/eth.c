#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <HAL/hal/hal.h>

#ifndef BSP_NB_DMA_IO_MAX
#define BSP_NB_DMA_IO_MAX 1
#endif

#include <libmppa_eth_core.h>
#include <libmppa_eth_loadbalancer_core.h>
#include <mppa_routing.h>
#include <mppa_noc.h>

#include "rpc.h"
#include "eth.h"

int eth_open_rx(unsigned remoteClus, odp_rpc_t *msg)
{
	odp_rpc_cmd_open_t data = { .inl_data = msg->inl_data };
	const uint32_t nocIf = remoteClus % 4;
	const uint32_t nocTx = ETH_BASE_TX + (remoteClus / 4);
	volatile mppa_dnoc_min_max_task_id_t *context;
	mppa_dnoc_header_t header = { 0 };
	mppa_dnoc_channel_config_t config = { 0 };
	int ret;


	/* Configure Tx */
	ret = mppa_routing_get_dnoc_unicast_route(__k1_get_cluster_id() + nocIf,
						  remoteClus, &config, &header);
	if (ret != MPPA_ROUTING_RET_SUCCESS)
		return 1;

	ret = mppa_noc_dnoc_tx_alloc(nocIf, nocTx);
	if (ret != MPPA_ROUTING_RET_SUCCESS)
		return 1;

	ret = mppa_noc_dnoc_tx_configure(nocIf, nocTx, header, config);
	if (ret != MPPA_ROUTING_RET_SUCCESS)
		goto open_err;

	context =  &mppa_dnoc[nocIf]->tx_chan_route[nocTx].
		min_max_task_id[ETH_DEFAULT_CTX];

	context->_.current_task_id = data.min_rx;
	context->_.min_task_id = data.min_rx;
	context->_.max_task_id = data.max_rx;
	context->_.min_max_task_id_en = 1;

	/* Configure dispatcher so that the defaulat "MATCH ALL" also
	 * sends packet to our cluster */
	mppabeth_lb_cfg_table_rr_dispatch_trigger((void *)&(mppa_ethernet[0]->lb),
						  ETH_MATCHALL_TABLE_ID,
						  data.ifId, 1);
	mppabeth_lb_cfg_table_rr_dispatch_channel((void *)&(mppa_ethernet[0]->lb),
						  ETH_MATCHALL_TABLE_ID,
						  data.ifId, nocIf, nocTx,
						  (1 << ETH_DEFAULT_CTX));
	return 0;

open_err:
	mppa_noc_dnoc_tx_free(nocIf, nocTx);
	return 1;
}

int eth_close_rx(unsigned remoteClus, odp_rpc_t *msg)
{
	odp_rpc_cmd_clos_t data = { .inl_data = msg->inl_data };
	const uint32_t nocIf = remoteClus % 4;
	const uint32_t nocTx = ETH_BASE_TX + (remoteClus / 4);

	/* Deconfigure DMA/Tx in the RR bitmask */
	mppabeth_lb_cfg_table_rr_dispatch_channel((void *)&(mppa_ethernet[0]->lb),
						  ETH_MATCHALL_TABLE_ID,
						  data.ifId, nocIf, nocTx,
						  (1 << ETH_DEFAULT_CTX));
	/* Close the Tx */
	mppa_noc_dnoc_tx_free(nocIf, nocTx);
	return 0;
}
void eth_init(void)
{
	/* "MATCH_ALL" Rule */
	mppabeth_lb_cfg_rule((void *)&(mppa_ethernet[0]->lb),
			     ETH_MATCHALL_TABLE_ID, ETH_MATCHALL_RULE_ID,
			     /* offset */ 0, /* Cmp Mask */0,
			     /* Espected Value */ 0, /* Hash. Unused */0);

	for (int ifId = 0; ifId < 4; ++ifId) {
		mppabeth_lb_cfg_header_mode((void *)&(mppa_ethernet[0]->lb),
					    ifId, MPPABETHLB_ADD_HEADER);
		mppabeth_lb_cfg_extract_table_mode((void *)&(mppa_ethernet[0]->lb),
						   0, 0, MPPABETHLB_DISPATCH_POLICY_RR);
	}
}

