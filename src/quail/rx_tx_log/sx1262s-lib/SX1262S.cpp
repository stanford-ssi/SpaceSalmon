#include "SX1262S.hpp"

SX1262S::SX1262S(Module* mod) : SX1262(mod) {

}

int16_t SX1262S::startTransmit(uint8_t *data, size_t len, uint8_t addr)
{
  // suppress unused variable warning
  (void)addr;

  // check packet length
  if (len > SX126X_MAX_PACKET_LENGTH)
  {
    return (ERR_PACKET_TOO_LONG);
  }

  // maximum packet length is decreased by 1 when address filtering is active
  if ((_addrComp != SX126X_GFSK_ADDRESS_FILT_OFF) && (len > SX126X_MAX_PACKET_LENGTH - 1))
  {
    return (ERR_PACKET_TOO_LONG);
  }

  // set packet Length
  int16_t state = ERR_NONE;
  uint8_t modem = getPacketType();
  if (modem == SX126X_PACKET_TYPE_LORA)
  {
    state = setPacketParams(_preambleLength, _crcType, len, _headerType);
  }
  else if (modem == SX126X_PACKET_TYPE_GFSK)
  {
    state = setPacketParamsFSK(_preambleLengthFSK, _crcTypeFSK, _syncWordLength, _addrComp, _whitening, _packetType, len);
  }
  else
  {
    return (ERR_UNKNOWN);
  }
  RADIOLIB_ASSERT(state);

  // set DIO mapping
  //state = setDioIrqParams(SX126X_IRQ_TX_DONE | SX126X_IRQ_TIMEOUT, SX126X_IRQ_TX_DONE);
  //RADIOLIB_ASSERT(state);

  // set buffer pointers
  state = setBufferBaseAddress();
  RADIOLIB_ASSERT(state);

  // write packet to buffer
  state = writeBuffer(data, len);
  RADIOLIB_ASSERT(state);

  // clear interrupt flags
  //state = clearIrqStatus();
  //RADIOLIB_ASSERT(state);

  // fix sensitivity
  state = fixSensitivity();
  RADIOLIB_ASSERT(state);

  // start transmission
  state = setTx(SX126X_TX_TIMEOUT_NONE);
  RADIOLIB_ASSERT(state);

  // wait for BUSY to go low (= PA ramp up done)
  while (digitalRead(_mod->getGpio()))
  {
    yield();
  }

  return (state);
}

int16_t SX1262S::startReceiveCommon()
{
  // set DIO mapping
  //int16_t state = setDioIrqParams(SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT | SX126X_IRQ_CRC_ERR | SX126X_IRQ_HEADER_ERR, SX126X_IRQ_RX_DONE);
  //RADIOLIB_ASSERT(state);

  // set buffer pointers
  int16_t state = setBufferBaseAddress();
  RADIOLIB_ASSERT(state);

  // clear interrupt flags
  //state = clearIrqStatus();

  // set implicit mode and expected len if applicable
  if (_headerType == SX126X_LORA_HEADER_IMPLICIT && getPacketType() == SX126X_PACKET_TYPE_LORA)
  {
    state = setPacketParams(_preambleLength, _crcType, _implicitLen, _headerType);
    RADIOLIB_ASSERT(state);
  }

  return (state);
}

int16_t SX1262S::startReceive(uint32_t timeout) {
  int16_t state = startReceiveCommon();
  RADIOLIB_ASSERT(state);

  // set mode to receive
  state = setRx(timeout);

  return(state);
}

uint32_t SX1262S::symbolToMs(uint32_t symbols)
{
  uint32_t symbolLength_us = ((uint32_t)(1000 * 10) << _sf) / (_bwKhz * 10);
  return symbols * symbolLength_us / 1000;
}