#pragma once
#include "hal_rtos.h"
#include "message_buffer.h"

template <typename T, int sz>
class MsgBuffer
{
private:
    uint8_t ucStorageBuffer[sz];
    StaticMessageBuffer_t xMessageBufferStruct;
    MessageBufferHandle_t xMessageBuffer;

public:
    MsgBuffer();
    bool send(T &data);
    bool receive(T &data, bool block);
    bool empty();
};

template <typename T, int sz>
MsgBuffer<T, sz>::MsgBuffer()
{
    xMessageBuffer = xMessageBufferCreateStatic(sizeof(ucStorageBuffer),
                                                ucStorageBuffer,
                                                &xMessageBufferStruct);
}

template <typename T, int sz>
//this is not blocking, and is safe to call from anywhere
bool MsgBuffer<T, sz>::send(T &data)
{
    vPortEnterCritical();
    size_t ret = xMessageBufferSend(xMessageBuffer, &data, sizeof(T), 0);
    vPortExitCritical();
    return (ret == sizeof(T));
}

template <typename T, int sz>
//WARN: this is not reentrant! Only call from one thread!
bool MsgBuffer<T, sz>::receive(T &data, bool block)
{
    size_t ret = xMessageBufferReceive(xMessageBuffer, &data, sizeof(T), block ? NEVER : 0);
    return (ret == sizeof(T));
}

template <typename T, int sz>
//WARN: this is not reentrant! Only call from one thread!
bool MsgBuffer<T, sz>::empty()
{
    return xMessageBufferIsEmpty(xMessageBuffer);
}