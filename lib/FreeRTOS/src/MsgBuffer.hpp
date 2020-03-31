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
    void send(T &data);
    void tryReceive(T &data);
    void receive(T &data);
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
void MsgBuffer<T, sz>::send(T &data)
{
    vPortEnterCritical();
    xMessageBufferSend(xMessageBuffer, &data, sizeof(T), 0);
    vPortExitCritical();
}

template <typename T, int sz>
//This is not blocking, and is safe to call from anywhere.
void MsgBuffer<T, sz>::tryReceive(T &data)
{
    vPortEnterCritical();
    xMessageBufferReceive(xMessageBuffer, &data, sizeof(T), 0);
    vPortExitCritical();
}

template <typename T, int sz>
//WARN: this will block forever, and is not reentrant! Only call from one place!
void MsgBuffer<T, sz>::receive(T &data)
{
    xMessageBufferReceive(xMessageBuffer, &data, sizeof(T), NEVER);
}