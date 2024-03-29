#ifndef __FIRMWARE__IOETH__RPC__H__
#define __FIRMWARE__IOETH__RPC__H__

#define RPC_BASE_RX 10
#define RPC_MAX_PAYLOAD 1024 /* max payload in bytes */

typedef struct {
	uint64_t data[4];
} odp_rpc_inl_data_t;

typedef struct {
	uint16_t pkt_type;
	uint16_t data_len;       /* Packet is data len * 8B long. data_len < RPC_MAX_PAYLOAD / 8 */
	uint8_t  dma_id;
	uint8_t  cnoc_id;
	odp_rpc_inl_data_t inl_data;
} odp_rpc_t;

typedef enum {
	ODP_RPC_CMD_INVL = 0 /**< Invalid command. Skip */,
	ODP_RPC_CMD_OPEN     /**< Forward Rx traffic to a cluster */,
	ODP_RPC_CMD_CLOS     /**< Stop forwarding Rx trafic to a cluster */
} odp_rpc_cmd_e;


typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
		uint8_t min_rx : 8;
		uint8_t max_rx : 8;
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_open_t;

typedef union {
	struct {
		uint8_t ifId : 3; /* 0-3, 4 for 40G */
	};
	odp_rpc_inl_data_t inl_data;
} odp_rpc_cmd_clos_t;

void rpcHandle(unsigned remoteClus, odp_rpc_t * msg);

#endif /* __FIRMWARE__IOETH__RPC__H__ */
