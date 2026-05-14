/*
------------------------------------------------------------------------------
~ File   : GSMCoreConf.hpp
~ Author : Majid Derhambakhsh
~ Version: V1.0.0
~ Created: 02/14/2021 06:00:00 AM
~ Brief  :
~ Support:
		   E-Mail : Majid.do16@gmail.com (subject : Embedded Library Support)

		   Github : https://github.com/Majid-Derhambakhsh
------------------------------------------------------------------------------
~ Description:

~ Attention  :

~ Changes    :
------------------------------------------------------------------------------
*/

#ifndef __GSM_CORE_CFG_H_
#define __GSM_CORE_CFG_H_

/* ~~~~~~~~~~~~~~ Required Headers ~~~~~~~~~~~~~ */
/* Driver-library for AVR */
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "timer.h"
#include "gpio_unit.h"

/* Driver-library for STM32 */
// #include "uart.h"
// #include "timer.h"

/* ~~~~~~~~~~~~ Function Overriding ~~~~~~~~~~~~ */
/* Driver-library for AVR */
#define GSM_GET_TICK()                                  SysTick_GetTick()
#define GSM_GPIO_WritePin(GPIOx, GPIO_Pin, PinState)    GPIO_WritePin((GPIOx), (GPIO_Pin), (PinState))
#define GSM_UART_Transmit(Data, Size)                   UART_Transmit((Data), (Size), 100)
#define GSM_UART_Transmit(Data, Size)                   UART1_Transmit_IT((Data), (Size));

/* Driver-library for STM32 */
// #define GSM_GET_TICK()                                    HAL_GetTick()
// #define GSM_GPIO_WritePin(GPIOx, GPIO_Pin, PinState)      HAL_GPIO_WritePin((GPIOx), (GPIO_Pin), (PinState))
// #define GSM_UART_Transmit(Data, Size)                     HAL_UART_Transmit(&huart1, (Data), (Size))
//#define GSM_UART_Transmit(Data, Size)                   HAL_UART_TransmitIT((Data), (Size));

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* __GSM_CORE_CFG_H_ */
