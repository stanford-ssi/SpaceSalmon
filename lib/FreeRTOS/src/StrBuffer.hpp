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
    void send(const char *data, size_t len);
    void tryReceive(const char *data, size_t len);
    void receive(const char *data, size_t len);
};

template <int sz>
StrBuffer<sz>::StrBuffer()
{
    xMessagebuffer = xMessageBufferCreateStatic(sizeof(ucStorageBuffer),
                                                ucStorageBuffer,
                                                &xMessageBufferStruct);
}

template <int sz>
//this is not blocking, and is safe to call from anywhere
void StrBuffer<sz>::send(const char *data, size_t len)
{
    vPortEnterCritical();
    xMessageBufferSend(xMessageBuffer, data, len, 0);
    vPortExitCritical();
}

template <int sz>
//this is not blocking, and is safe to call from anywhere
void StrBuffer<sz>::tryReceive(const char *data, size_t len)
{
    vPortEnterCritical();
    xMessageBufferReceive(xMessageBuffer, data, len, 0);
    vPortExitCritical();
}

template <int sz>
//WARN: this will block forever, and is not reentrant! Only call from one place!
void StrBuffer<sz>::receive(const char *data, size_t len)
{
    xMessageBufferReceive(xMessageBuffer, data, len, NEVER);
}