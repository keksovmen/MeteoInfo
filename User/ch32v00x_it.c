/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v00x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include <ch32v00x_it.h>

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
	while (1)
	{
	}
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
	printf("Hard fault");
	// Read the critical registers
	uint32_t mepc_val = __get_MEPC();
	uint32_t mcause_val = __get_MCAUSE();
	// uint32_t mtval_val = __get_MTVAL();

	// Print or use the values for debugging
	printf("HardFault occurred!\r\n");
	printf("mepc  : 0x%08x\r\n", mepc_val);
	printf("mcause: 0x%08x\r\n", mcause_val);
	// printf("mtval : 0x%08x\r\n", mtval_val);

	switch(mcause_val)
	{
		case 0:
			printf("Instruction address miss aligned\r\n");
			break;
		case 1:
			printf("Fetch command access error\r\n");
			break;
		case 2:
			printf("Illegal instruction\r\n");
			break;
		case 3:
			printf("Breakpoints\r\n");
			break;
		case 4:
			printf("Load instruction access address misalignment\r\n");
			break;
		case 5:
			printf("Load command access error\r\n");
			break;
		case 6:
			printf("Store/AMO instruction access address misalignment\r\n");
			break;
		case 7:
			printf("Store/AMO command access error\r\n");
			break;

		default: break;
	}

	while (1)
	{
	}
}
