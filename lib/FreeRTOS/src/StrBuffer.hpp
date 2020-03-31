#pragma once
#include "hal_rtos.h"
#include "message_buffer.h"

template <int sz>
class StrBuffer
{
private:
    uint8_t ucStorageBuffer[sz];
    StaticMessageBuffer_t xMessageBufferStruct;
    MessageBufferHandle_t xMessageBuffer;

public:
    StrBuffer();
    size_t send(const char *data, size_t len);
    size_t receive(char *data, size_t len, bool block);
};

template <int sz>
StrBuffer<sz>::StrBuffer()
{
    xMessageBuffer = xMessageBufferCreateStatic(sizeof(ucStorageBuffer),
                                                ucStorageBuffer,
                                                &xMessageBufferStruct);
}

template <int sz>
//this is not blocking, and is safe to call from anywhere
size_t StrBuffer<sz>::send(const char *data, size_t len)
{
    vPortEnterCritical();
    size_t ret = xMessageBufferSend(xMessageBuffer, data, len, 0);
    vPortExitCritical();
    return ret;
}

template <int sz>
//WARN: this is not reentrant! Only call from one thread!
size_t StrBuffer<sz>::receive(char *data, size_t len, bool block)
{
    return xMessageBufferReceive(xMessageBuffer, data, len, block ? NEVER : 0);
}