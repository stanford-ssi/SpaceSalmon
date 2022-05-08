#include "main.hpp"

void HardFault_Handler(void)
{
	__BKPT(3);
}

void MemManage_Handler(void)
{
	__BKPT(3);
}

void BusFault_Handler(void)
{
	__BKPT(3);
}
