/*
------------------------------------------------------------------------------
~ File   : GSMCore.cpp
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

#ifndef __GSM_CORE_H_
#define __GSM_CORE_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "GSMCoreConf.hpp"

using namespace std;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ------------------------ Buffer ------------------------- */
#define GSM_BUFFER_SIZE    200
//#define GSM_CMD_RESP_SIZE  100

/* ------------------------ Timing ------------------------- */
/* Millisecond */
#define GSM_TIMOUT_MS         2000
#define GSM_TIMOUT_DEF_MS     10000
#define GSM_UART_TIMEOUT      100

/* ---------------------- by Compiler ---------------------- */
#ifndef USE_HAL_DRIVER
	
	#define GPIO_PIN_SET    _GPIO_PIN_SET
	#define GPIO_PIN_RESET  _GPIO_PIN_RESET
	
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//typedef UART_HandleTypeDef UART_HandleTypeDef

typedef enum
{
	
	GSM_ERROR           = 0,
	GSM_OK              = 1,
	GSM_TIMOUT          = 2,
	GSM_CALL_NO_CARRIER = 3,
	GSM_CALL_BUSY       = 4,
	
}GSM_StatusTypeDef;

#ifndef USE_HAL_DRIVER
	
	typedef uint8_t GPIOPin_TypeDef;
	typedef volatile uint8_t* GPIO_TypeDef;
	
#endif

typedef void (*GSM_CallbackTypeDef)(GSM_StatusTypeDef Status);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//template <class ModemType>

class GSMCore
{
	
	private:
	
		/* ~~~~~~~~~~~~~~~~~~~~ Variable ~~~~~~~~~~~~~~~~~~~~~ */
		/* ... Data ... */
		volatile uint32_t _uartDataCounter;
		volatile uint32_t _dataTransmitLength;
	
		/* ... Timing ... */
		volatile uint32_t _timeOut;
		volatile uint32_t _startTimeOut;
		volatile uint8_t  _timeoutState;
	
		/* ... GPIO ... */
		GPIO_TypeDef    _powerGPIOPort;
		GPIOPin_TypeDef _powerGPIOPin;
		
		/* ... Buffer ... */
		char _tBuff[GSM_BUFFER_SIZE];
		char _rBuff[GSM_BUFFER_SIZE];
		char _smsBuff[GSM_BUFFER_SIZE];
		//char _cmdResp[GSM_CMD_RESP_SIZE];
		
		/* ... Events ... */
		GSM_CallbackTypeDef _userCallback;
		
		void(GSMCore::*DataHandle)(void);
		
		/* ~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~ */
		/* ::::::::::::::::: Buffer :::::::::::::::::: */
		void WriteToTransmitBuffer(char *Data);
		
		void Replace(char *Source, const char *DataToReplace, const char DataToWrite);
		
		void Trim(char *Source);
		
		/* ::::::::::::::::: Timing :::::::::::::::::: */
		void SetTimeOut(uint32_t Timout);
		
		/* ::::::::::::::::: Events :::::::::::::::::: */
		/* ... Global ... */
		void NullEvent();
		
		void OKResponseEvent(void);
		
		/* ... SMS ... */
		void SMS_InitEvent(void);
		
		void SMS_InitHexEvent(void);
		
		void SMS_SendEvent(void);
		
		/* ... Call ... */
		void Call_StatusEvent(void);
		
	public:
		
		/* ~~~~~~~~~~~~~~~~~~~~ Variable ~~~~~~~~~~~~~~~~~~~~~ */
		
		/* ~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~ */
		/* ::::::::::::::::: Buffer :::::::::::::::::: */
		void ClearReceiveBuffer(void);
		void ClearTransmitBuffer(void);
		
		/* ::::::::::::::: Initializer ::::::::::::::: */
		void SetPowerGPIO(GPIO_TypeDef GPIOx, GPIOPin_TypeDef GPIO_Pin);
		
		void Init(void);
		
		/* ::::::::::::::::: Handle :::::::::::::::::: */
		void UARTHandle(uint8_t *Data);
		
		void IsReceived(char *cmd, GSM_CallbackTypeDef UserCallback);
		
		void TimingHandle(void);
		
		/* :::::::::::::: Data Transmit :::::::::::::: */
		template <typename... Args>
		
		void Transmit(Args... Data);
		
		/* ... AT Command ... */
		template <typename... Args>
		
		void TransmitAT(Args... CMD);
		
		/* :::::::::::::: Modem Control :::::::::::::: */
		/* ... Modem Power ... */
		void PowerOn();
		
		void PowerOff();
		
		/* ... Modem Echo ... */
		void DisableEcho(GSM_CallbackTypeDef UserCallback);
		
		/* ... Modem Test ... */
		void TestAT(GSM_CallbackTypeDef UserCallback);
		
		/* ... Modem Info ... */
		void GetModemInfo(GSM_CallbackTypeDef UserCallback);
		
		char *ReadModemName(void);
		
		/* ... GSM ... */
		void SetPhoneFunctionality(GSM_CallbackTypeDef UserCallback);
		
		/* ... SIMCARD ... */
		void SIMCARD_EnterPin(GSM_CallbackTypeDef UserCallback);
		
		uint8_t SIMCARD_SIMStatus(void);
		
		/* ... Modem SMS .... */
		void SMS_Init(GSM_CallbackTypeDef UserCallback);
		
		void SMS_InitHex(GSM_CallbackTypeDef UserCallback);
		
		char *SMS_ReadData(void);
		
		void SMSTo(GSM_CallbackTypeDef UserCallback, char *Number, char *Text);
		
		/* ... Modem Call .... */
		void Call_Init(GSM_CallbackTypeDef UserCallback);
		
		char *Call_ReadData(void);
		void Call_Answer(void);
		void Call_Reject(void);
		void CallTo(GSM_CallbackTypeDef UserCallback, char *Number);
		
		/* ... Operator ... */
		void GetOperatorInfo(GSM_CallbackTypeDef UserCallback);
		
		char *ReadOperatorCode(void);
		
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* __GSM_CORE_H_ */
