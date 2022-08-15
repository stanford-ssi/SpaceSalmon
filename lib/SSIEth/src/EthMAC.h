#pragma once
#include <compiler.h>
#include <utils.h>
#include "return.h"
#undef ERR_TIMEOUT // I hate this language
#include "lwip/pbuf.h"

struct mac_async_filter
{
	uint8_t mac[6];	 /*!< Destination address */
	uint8_t tid[2];	 /*!< Type ID, 0x0600 IP package */
	bool tid_enable; /*!< Enable TID matching */
};

/**
 * \brief MAC callback types
 */
enum mac_async_cb_type
{
	MAC_ASYNC_RECEIVE_CB, /*!< One or more frame been received */
	MAC_ASYNC_TRANSMIT_CB /*!< One or more frame been transmited */
};

typedef void (*FUNC_PTR_OBJ)(void *);

class EthMAC
{
private:
	FUNC_PTR_OBJ transmited; /*!< Frame received */
	void *tx_cb_obj = nullptr;

	FUNC_PTR_OBJ received; /*!< Frame transmited */
	void *rx_cb_obj = nullptr;

	result_t _init_bufdescr();

public:
	const Gmac *hw; /*!< Hardware module instance handler */
	EthMAC(const Gmac *_hw) : hw(_hw){};
	result_t init();
	result_t deinit();
	result_t enable();
	result_t disable();
	result_t write(struct pbuf *p);
	result_t read(uint8_t *buf, uint32_t len, uint32_t &rx_len);
	result_t read_len(uint32_t &len);
	result_t register_callback(const enum mac_async_cb_type type, const FUNC_PTR_OBJ func, void *obj);
	result_t set_filter(uint8_t index, struct mac_async_filter *filter);
	result_t set_filter_ex(uint8_t mac[6]);
	result_t write_phy_reg(uint16_t addr, uint16_t reg, uint16_t data);
	result_t read_phy_reg(uint16_t addr, uint16_t reg, uint16_t *val);
	void handler();
};

/**
 * @brief Transmit buffer descriptor
 **/
struct _mac_txbuf_descriptor
{
	uint8_t *address;
	union gmac_tx_status
	{
		uint32_t val;
		struct _gmac_tx_status_bm
		{
			uint32_t len : 14,					 /**< Length of buffer */
				reserved : 1, last_buf : 1,		 /**< Last buffer (in the current frame) */
				no_crc : 1,						 /**< No CRC */
				reserved1 : 3, checksum_err : 3, /**< Transmit checksum generation offload error */
				reserved2 : 3, lco : 1,			 /**< Late collision, transmit error detected */
				exhausted : 1,					 /**< Buffer exhausted in mid frame */
				reserved3 : 1, error : 1,		 /**< Retry limit exceeded, error detected */
				wrap : 1,						 /**< Marks last descriptor in TD list */
				used : 1;						 /**< User clear, GMAC sets this to 1 once a frame
													  has been successfully transmitted */
		} bm;
	} status;
};

/**
 * @brief Receive buffer descriptor
 **/
struct _mac_rxbuf_descriptor
{
	union _gmac_rx_addr
	{
		uint32_t val;
		struct rx_addr_bm
		{
			uint32_t ownership : 1, /**< clear before buffer can be used again */
				wrap : 1,			/**< Marks last entry in array */
				addr : 30;			/**< Address in number of DW */
		} bm;
	} address;
	union gmac_rx_status
	{
		uint32_t val;
		struct _gmac_rx_status_bm
		{
			uint32_t len : 13,					  /**< Length of frame including FCS */
				fcs : 1,						  /**< Frame has bad FCS */
				sof : 1,						  /**< Start of frame */
				eof : 1,						  /**< End of frame */
				cfi : 1,						  /**< Concatenation Format Indicator */
				vlan_priority : 3,				  /**< VLAN priority (if VLAN detected) */
				priority_detected : 1,			  /**< Priority tag detected */
				vlan_detected : 1,				  /**< VLAN tag detected */
				type_id_match : 2,				  /**< Type ID match */
				checksumoffload : 1,			  /**< Checksum offload specific function */
				addrmatch : 2,					  /**< Address register match */
				ext_addr_match : 1,				  /**< External address match found */
				reserved : 1, uni_hash_match : 1, /**< Unicast hash match */
				multi_hash_match : 1,			  /**< Multicast hash match */
				boardcast_detect : 1;			  /**< Global broadcast address detected */
		} bm;
	} status;
};