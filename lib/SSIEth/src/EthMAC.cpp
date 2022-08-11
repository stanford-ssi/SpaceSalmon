#include <string.h>
#include "utils/utils_assert.h"
#include "EthMAC.h"
#include <hpl_gmac_config.h>

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) > (y) ? (x) : (y))

/* Transmit and Receive buffer descriptor array */
COMPILER_ALIGNED(8) static struct _mac_txbuf_descriptor _txbuf_descrs[CONF_GMAC_TXDESCR_NUM];
COMPILER_ALIGNED(8) static struct _mac_rxbuf_descriptor _rxbuf_descrs[CONF_GMAC_RXDESCR_NUM];

/* Transmit buffer data array */
COMPILER_ALIGNED(32)
static uint8_t _txbuf[CONF_GMAC_TXDESCR_NUM][CONF_GMAC_TXBUF_SIZE];
COMPILER_ALIGNED(32)
static uint8_t _rxbuf[CONF_GMAC_RXDESCR_NUM][CONF_GMAC_RXBUF_SIZE];

COMPILER_PACK_RESET()

/*!< Pointer to hpl device */
static EthMAC* _gmac_dev = nullptr;

/* Transmit and receive Buffer index */
static volatile uint32_t _txbuf_index;
static volatile uint32_t _last_txbuf_index;
static volatile uint32_t _rxbuf_index;

/**
 * \internal Initialize the Transmit and receive buffer descriptor array
 *
 * \param[in] dev Pointer to the HPL MAC descriptor
 */
result_t EthMAC::_init_bufdescr()
{
	uint32_t i;

	/* TX buffer descriptor */
	for (i = 0; i < CONF_GMAC_TXDESCR_NUM; i++) {
		_txbuf_descrs[i].address        = (uint32_t)_txbuf[i];
		_txbuf_descrs[i].status.val     = 0;
		_txbuf_descrs[i].status.bm.used = 1;
	}

	_txbuf_descrs[CONF_GMAC_TXDESCR_NUM - 1].status.bm.wrap = 1;
	_txbuf_index                                            = 0;
	_last_txbuf_index                                       = 0;

	/* RX buffer descriptor */
	for (i = 0; i < CONF_GMAC_RXDESCR_NUM; i++) {
		_rxbuf_descrs[i].address.val = (uint32_t)_rxbuf[i];
		_rxbuf_descrs[i].status.val  = 0;
	}

	_rxbuf_descrs[CONF_GMAC_RXDESCR_NUM - 1].address.bm.wrap = 1;
	_rxbuf_index                                             = 0;

	hri_gmac_write_TBQB_reg(hw, (uint32_t)_txbuf_descrs);
	hri_gmac_write_RBQB_reg(hw, (uint32_t)_rxbuf_descrs);
	
	return RET::OK;
}

/*
 * \internal GMAC interrupt handler
 */
void GMAC_Handler(void)
{
	if(_gmac_dev != nullptr){
		_gmac_dev->handler();
	}
}

void EthMAC::handler(){
	volatile uint32_t tsr;
	volatile uint32_t rsr;

	tsr = hri_gmac_read_TSR_reg(hw);
	rsr = hri_gmac_read_RSR_reg(hw);
	/* Must be Clear ISR (Clear on read) */
	hri_gmac_read_ISR_reg(hw);

	/* Frame transmited */
	if (tsr & GMAC_TSR_TXCOMP) {
		hri_gmac_write_TSR_reg(hw, tsr);
		if ((_txbuf_descrs[_txbuf_index].status.bm.used) && (transmited != NULL)) {
			transmited();
		}
	}

	/* Frame received */
	if (rsr & GMAC_RSR_REC) {
		if (received != NULL) {
			received();
		}
	}
	hri_gmac_write_RSR_reg(hw, rsr);
}

result_t EthMAC::init()
{
	hri_gmac_write_NCR_reg(hw,
	                       (CONF_GMAC_NCR_LBL ? GMAC_NCR_LBL : 0) | (CONF_GMAC_NCR_MPE ? GMAC_NCR_MPE : 0)
	                           | (CONF_GMAC_NCR_WESTAT ? GMAC_NCR_WESTAT : 0) | (CONF_GMAC_NCR_BP ? GMAC_NCR_BP : 0)
	                           | (CONF_GMAC_NCR_ENPBPR ? GMAC_NCR_ENPBPR : 0)
	                           | (CONF_GMAC_NCR_TXPBPF ? GMAC_NCR_TXPBPF : 0));
	hri_gmac_write_NCFGR_reg(
	    hw,
	    (CONF_GMAC_NCFGR_SPD ? GMAC_NCFGR_SPD : 0) | (CONF_GMAC_NCFGR_FD ? GMAC_NCFGR_FD : 0)
	        | (CONF_GMAC_NCFGR_DNVLAN ? GMAC_NCFGR_DNVLAN : 0) | (CONF_GMAC_NCFGR_JFRAME ? GMAC_NCFGR_JFRAME : 0)
	        | (CONF_GMAC_NCFGR_CAF ? GMAC_NCFGR_CAF : 0) | (CONF_GMAC_NCFGR_NBC ? GMAC_NCFGR_NBC : 0)
	        | (CONF_GMAC_NCFGR_MTIHEN ? GMAC_NCFGR_MTIHEN : 0) | (CONF_GMAC_NCFGR_UNIHEN ? GMAC_NCFGR_UNIHEN : 0)
	        | (CONF_GMAC_NCFGR_MAXFS ? GMAC_NCFGR_MAXFS : 0) | (CONF_GMAC_NCFGR_RTY ? GMAC_NCFGR_RTY : 0)
	        | (CONF_GMAC_NCFGR_PEN ? GMAC_NCFGR_PEN : 0) | GMAC_NCFGR_RXBUFO(CONF_GMAC_NCFGR_RXBUFO)
	        | (CONF_GMAC_NCFGR_LFERD ? GMAC_NCFGR_LFERD : 0) | (CONF_GMAC_NCFGR_RFCS ? GMAC_NCFGR_RFCS : 0)
	        | GMAC_NCFGR_CLK(CONF_GMAC_NCFGR_CLK) | (CONF_GMAC_NCFGR_DCPF ? GMAC_NCFGR_DCPF : 0)
	        | (CONF_GMAC_NCFGR_RXCOEN ? GMAC_NCFGR_RXCOEN : 0) | (CONF_GMAC_NCFGR_EFRHD ? GMAC_NCFGR_EFRHD : 0)
	        | (CONF_GMAC_NCFGR_IRXFCS ? GMAC_NCFGR_IRXFCS : 0) | (CONF_GMAC_NCFGR_IPGSEN ? GMAC_NCFGR_IPGSEN : 0)
	        | (CONF_GMAC_NCFGR_RXBP ? GMAC_NCFGR_RXBP : 0) | (CONF_GMAC_NCFGR_IRXER ? GMAC_NCFGR_IRXER : 0));
	hri_gmac_write_UR_reg(hw, (CONF_GMAC_UR_MII ? GMAC_UR_MII : 0));
	hri_gmac_write_DCFGR_reg(
	    hw,
	    GMAC_DCFGR_FBLDO(CONF_GMAC_DCFGR_FBLDO) | (CONF_GMAC_DCFGR_ESMA ? GMAC_DCFGR_ESMA : 0)
	        | (CONF_GMAC_DCFGR_ESPA ? GMAC_DCFGR_ESPA : 0) | GMAC_DCFGR_RXBMS(CONF_GMAC_DCFGR_RXBMS)
	        | (CONF_GMAC_DCFGR_TXPBMS ? GMAC_DCFGR_TXPBMS : 0) | (CONF_GMAC_DCFGR_TXCOEN ? GMAC_DCFGR_TXCOEN : 0)
	        | GMAC_DCFGR_DRBS(CONF_GMAC_DCFGR_DRBS) | (CONF_GMAC_DCFGR_DDRP ? GMAC_DCFGR_DDRP : 0));
	hri_gmac_write_WOL_reg(hw, 0);
	hri_gmac_write_IPGS_reg(hw, GMAC_IPGS_FL((CONF_GMAC_IPGS_FL_MUL << 8) | CONF_GMAC_IPGS_FL_DIV));
	_init_bufdescr();

	_gmac_dev = this;
	NVIC_DisableIRQ(GMAC_IRQn);
	NVIC_ClearPendingIRQ(GMAC_IRQn);
	NVIC_EnableIRQ(GMAC_IRQn);

	return RET::OK;
}

result_t EthMAC::deinit()
{
	/* Disable all GMAC Interrupt */
	hri_gmac_clear_IMR_reg(hw, 0xFFFFFFFF);
	/* Disable transmit/receive */
	hri_gmac_write_NCR_reg(hw, 0x0);
	hw = NULL;
	/* Disable Interrupt */
	NVIC_DisableIRQ(GMAC_IRQn);

	return RET::OK;
}

result_t EthMAC::enable()
{
	hri_gmac_set_NCR_reg(hw, GMAC_NCR_RXEN | GMAC_NCR_TXEN);
	return RET::OK;
}

result_t EthMAC::disable()
{
	hri_gmac_clear_NCR_reg(hw, GMAC_NCR_RXEN | GMAC_NCR_TXEN);
	return RET::OK;
}

result_t EthMAC::write(uint8_t *buf, uint32_t len)
{
	uint32_t pos;
	uint32_t blen;
	uint32_t i;

	if (_txbuf_descrs[_last_txbuf_index].status.bm.used && !_txbuf_descrs[_last_txbuf_index].status.bm.last_buf) {
		/* Set used flag from first descriptor to last descriptor,
		 * as DMA olny set the first used flag */
		for (i = 1; i < CONF_GMAC_TXDESCR_NUM; i++) {
			pos = _last_txbuf_index + i;
			if (pos >= CONF_GMAC_TXDESCR_NUM) {
				pos -= CONF_GMAC_TXDESCR_NUM;
			}
			_txbuf_descrs[pos].status.bm.used = 1;

			if (_txbuf_descrs[pos].status.bm.last_buf) {
				break;
			}
		}
	}

	if (!_txbuf_descrs[_txbuf_index].status.bm.used) {
		return RET::FAIL;
	}

	/* Check if have enough buffers, the first buffer already checked */
	if (len > CONF_GMAC_TXBUF_SIZE) {
		for (i = 1; i < CONF_GMAC_TXDESCR_NUM; i++) {
			pos = _txbuf_index + i;
			if (pos >= CONF_GMAC_TXDESCR_NUM) {
				pos -= CONF_GMAC_TXDESCR_NUM;
			}

			if (!_txbuf_descrs[pos].status.bm.used) {
				return RET::FAIL;
			}

			if ((len - (CONF_GMAC_TXBUF_SIZE * i)) < CONF_GMAC_TXBUF_SIZE) {
				break;
			}
		}
	}
	_last_txbuf_index = _txbuf_index;

	/* Write data to transmit buffer */
	for (i = 0; i < CONF_GMAC_TXDESCR_NUM; i++) {
		blen = min(len, CONF_GMAC_TXBUF_SIZE);
		memcpy(_txbuf[_txbuf_index], buf + (i * CONF_GMAC_TXBUF_SIZE), blen);
		len -= blen;

		if (len > 0) {
			/* Here the Used flag be set to zero */
			_txbuf_descrs[_txbuf_index].status.val = blen;
		} else {
			_txbuf_descrs[_txbuf_index].status.val         = blen;
			_txbuf_descrs[_txbuf_index].status.bm.last_buf = 1;
		}
		_txbuf_index++;
		if (_txbuf_index == CONF_GMAC_TXDESCR_NUM) {
			_txbuf_index                                            = 0;
			_txbuf_descrs[CONF_GMAC_TXDESCR_NUM - 1].status.bm.wrap = 1;
		}
		if (len == 0) {
			break;
		}
	}

	/* Data synchronization barrier */
	__DSB();

	/* Active Transmit */
	hri_gmac_set_NCR_reg(hw, GMAC_NCR_TSTART);

	return RET::OK;
}

result_t EthMAC::read(uint8_t *buf, uint32_t buf_len, uint32_t &rx_len)
{
	uint32_t i;
	uint32_t j;
	uint32_t pos;
	uint32_t n;
	uint32_t sof       = 0xFFFFFFFF; /* Start of Frame index */
	uint32_t eof       = 0xFFFFFFFF; /* End of Frame index */
	uint32_t total_len = 0;          /* Total length of received package */

	for (i = 0; i < CONF_GMAC_RXDESCR_NUM; i++) {
		pos = _rxbuf_index + i;

		if (pos >= CONF_GMAC_RXDESCR_NUM) {
			pos -= CONF_GMAC_RXDESCR_NUM;
		}

		/* No more data for Ethernet package */
		if (!_rxbuf_descrs[pos].address.bm.ownership) {
			break;
		}

		if (_rxbuf_descrs[pos].status.bm.sof) {
			sof = i;
		}

		if ((_rxbuf_descrs[pos].status.bm.eof) && (sof != 0xFFFFFFFF)) {
			/* eof now indicate the number of bufs the frame used */
			eof = i;
			n   = _rxbuf_descrs[pos].status.bm.len;
			buf_len = min(n, buf_len);
			/* Break process since the last data has been found */
			break;
		}
	}

	if (eof != 0xFFFFFFFF) {
		j = eof + 1;
	} else if (sof != 0xFFFFFFFF) {
		j = sof;
	} else {
		j = i;
	}

	/* Copy data to user buffer */
	for (i = 0; i < j; i++) {
		if (eof != 0xFFFFFFFF && i >= sof && i <= eof && buf_len > 0) {
			n = min(buf_len, CONF_GMAC_RXBUF_SIZE);
			memcpy(buf, _rxbuf[_rxbuf_index], n);
			buf += n;
			total_len += n;
			buf_len -= n;
		}

		_rxbuf_descrs[_rxbuf_index].address.bm.ownership = 0;
		_rxbuf_index++;

		if (_rxbuf_index == CONF_GMAC_RXDESCR_NUM) {
			_rxbuf_index = 0;
		}
	}

	rx_len = total_len;
	return RET::OK;
}

result_t EthMAC::read_len(uint32_t &len)
{
	uint32_t i;
	uint32_t pos;
	bool     sof       = false; /* Start of Frame */
	uint32_t total_len = 0;     /* Total length of received package */

	for (i = 0; i < CONF_GMAC_RXDESCR_NUM; i++) {
		pos = _rxbuf_index + i;

		if (pos >= CONF_GMAC_RXDESCR_NUM) {
			pos -= CONF_GMAC_RXDESCR_NUM;
		}

		/* No more data for Ethernet package */
		if (!(_rxbuf_descrs[pos].address.bm.ownership)) {
			break;
		}

		if (_rxbuf_descrs[pos].status.bm.sof) {
			sof = true;
		}
		if (sof == true) {
			total_len += _rxbuf_descrs[pos].status.bm.len;
		}

		if (_rxbuf_descrs[pos].status.bm.eof) {
			/* Break process since the last data has been found */
			break;
		}
	}

	len = total_len;
	return RET::OK;
}

void EthMAC::enable_irq()
{
	NVIC_EnableIRQ(GMAC_IRQn);
}

void EthMAC::disable_irq()
{
	NVIC_DisableIRQ(GMAC_IRQn);
}

result_t EthMAC::register_callback(const enum mac_async_cb_type type,
                                     const FUNC_PTR func)
{
	switch (type) {
	case MAC_ASYNC_TRANSMIT_CB:
		transmited = func;
		if (func) {
			hri_gmac_set_IMR_TCOMP_bit(hw);
		} else {
			hri_gmac_clear_IMR_TCOMP_bit(hw);
		}
		break;
	case MAC_ASYNC_RECEIVE_CB:
		received = func;
		if (func) {
			hri_gmac_set_IMR_RCOMP_bit(hw);
		} else {
			hri_gmac_set_IMR_RCOMP_bit(hw);
		}
		break;
	default:
		return RET::FAIL;
	}
	return RET::OK;
}

result_t EthMAC::set_filter(uint8_t index, struct mac_async_filter *filter)
{
	ASSERT(index < 4);

	hri_gmac_write_SAB_reg(hw, index, *((uint32_t *)(filter->mac)));
	hri_gmac_write_SAT_reg(hw, index, *((uint16_t *)(filter->mac + 4)));
	hri_gmac_write_TIDM_reg(hw, index, GMAC_TIDM_TID(*((uint16_t *)(filter->tid)) | filter->tid_enable << 31));
	
	return RET::OK;
}

result_t EthMAC::set_filter_ex(uint8_t mac[6])
{
	uint8_t j;
	uint8_t m;
	uint8_t n;
	uint8_t k = 0;

	/* Apply the hash function */
	for (j = 0; j < 48; j += 6) {
		/* Calculate the shift count */
		n = j / 8;
		m = j % 8;

		/* Update hash value */
		if (!m) {
			k ^= mac[n];
		} else {
			k ^= (mac[n] >> m) | (mac[n + 1] << (8 - m));
		}
	}

	/* The hash value is reduced to a 6-bit index */
	k &= 0x3F;

	if (k < 32) {
		hri_gmac_set_HRB_reg(hw, 1 << k);
	} else {
		hri_gmac_set_HRT_reg(hw, 1 << (k % 32));
	}

	return RET::OK;
}

result_t EthMAC::write_phy_reg(uint16_t addr, uint16_t reg, uint16_t data)
{
	hri_gmac_set_NCR_reg(hw, GMAC_NCR_MPE);
	hri_gmac_write_MAN_reg(hw,
	                       GMAC_MAN_OP(1) |            /* 0x01 write operation */
	                           CONF_GMAC_CLTTO << 30 | /* Clause 22/45 operation */
	                           GMAC_MAN_WTN(2) |       /* Must be written to 0x2 */
	                           GMAC_MAN_PHYA(addr) | GMAC_MAN_REGA(reg) | GMAC_MAN_DATA(data));
	/* Wait for the write operation complete */
	while (!hri_gmac_get_NSR_IDLE_bit(hw)) {
	}

	hri_gmac_clear_NCR_reg(hw, GMAC_NCR_MPE);

	return RET::OK;
}

result_t EthMAC::read_phy_reg(uint16_t addr, uint16_t reg, uint16_t *data)
{
	hri_gmac_set_NCR_reg(hw, GMAC_NCR_MPE);
	hri_gmac_write_MAN_reg(hw,
	                       GMAC_MAN_OP(2) |            /* 0x02 read operation */
	                           CONF_GMAC_CLTTO << 30 | /* Clause 22/45 operation */
	                           GMAC_MAN_WTN(0x2) |     /* Must be written to 0x2 */
	                           GMAC_MAN_PHYA(addr) | GMAC_MAN_REGA(reg));

	/* Wait for the read operation complete */
	while (!hri_gmac_get_NSR_IDLE_bit(hw)) {
	}

	*data = GMAC_MAN_DATA(hri_gmac_read_MAN_reg(hw));
	hri_gmac_clear_NCR_reg(hw, GMAC_NCR_MPE);

	return RET::OK;
}
