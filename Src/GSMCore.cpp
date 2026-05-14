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

#include "GSMCore.hpp"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
const char PROGMEM GSM_RESP_OK[] = "OK\r\n";
const char PROGMEM GSM_RESP_ERROR[] = "ERROR\r\n";
const char PROGMEM GSM_RESP_NO_CARRIER[] = "NO CARRIER";
const char PROGMEM GSM_RESP_BUSY[] = "BUSY";
const char PROGMEM GSM_RESP_WAIT_TO_TEXT[] = ">";

const char PROGMEM GSM_RESP_FOR_REMOVE_RN_OK_RN[] = "\r\nOK\r\n";
const char PROGMEM GSM_RESP_FOR_REMOVE_R_OK_R[] = "\rOK\r";
const char PROGMEM GSM_RESP_FOR_REMOVE_RN[] = "\r\n";
const char PROGMEM GSM_RESP_FOR_REMOVE_R[] = "\r";
const char PROGMEM GSM_RESP_FOR_REMOVE_RN_ERROR_RN[] = "\r\nERROR\r\n";
const char PROGMEM GSM_RESP_FOR_REMOVE_R_ERROR_R[] = "\rERROR\r";
const char PROGMEM GSM_RESP_FOR_REMOVE_RN_NO_CARRIER_RN[] = "\r\nNO CARRIER\r\n";
const char PROGMEM GSM_RESP_FOR_REMOVE_R_NO_CARRIER_R[] = "\rNO CARRIER\r";
const char PROGMEM GSM_RESP_FOR_REMOVE_RN_BUSY_RN[] = "\r\nBUSY\r\n";
const char PROGMEM GSM_RESP_FOR_REMOVE_R_BUSY_R[] = "\rBUSY\r";

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ::::::::::::::::: Buffer :::::::::::::::::: */
void GSMCore::ClearReceiveBuffer(void)
{
	
	if (this->_uartDataCounter > 0)
	{
		
		/* ~~~~~ Clear UART received data ~~~~ */
		memset(this->_rBuff, '\0', this->_uartDataCounter);
		
		/* ~~~~~ Reset UART data counter ~~~~~ */
		this->_uartDataCounter = 0;
		
	}
	
}

void GSMCore::ClearTransmitBuffer(void)
{
	
	if (this->_dataTransmitLength > 0)
	{
		
		/* ~~~~~ Clear UART transmit data ~~~~ */
		memset(this->_tBuff, '\0', this->_dataTransmitLength);
		
		/* ~~~~~ Reset UART data length ~~~~~~ */
		this->_dataTransmitLength = 0;
		
	}
	
}

void GSMCore::WriteToTransmitBuffer(char *Data)
{
	
	strcpy(&this->_tBuff[this->_dataTransmitLength], Data);
	
	this->_dataTransmitLength += strlen(Data);
	
}

void GSMCore::Replace(char *Source, const char *DataToReplace, const char DataToWrite)
{
	
	char *indx = strstr_P(Source, DataToReplace);
	
	//size_t dataSize = strlen(DataToReplace);
	
	if (indx != NULL)
	{
		memset(indx, DataToWrite, strlen_P(DataToReplace));
	}
	
}

void GSMCore::Trim(char* Source)
{

	/* Find first and last space in array */
	int16_t strLength          = strlen(Source);
	int16_t lastSpaceFromStart = strspn(Source, " "); // Last space from start
	int16_t lastSpaceFromEnd   = strLength - 1;
	int16_t sizeToMove;
	
	while ((lastSpaceFromEnd > 0) && (Source[lastSpaceFromEnd] == ' '))
	{
		lastSpaceFromEnd--;
	}
	
	/* Shift correct data to first of array */
	sizeToMove = (++lastSpaceFromEnd - lastSpaceFromStart);
	
	if (sizeToMove > 0)
	{
		memmove(Source, &Source[lastSpaceFromStart], sizeToMove);
	}
	
	/* Erase remaining data */
	strLength = strlen(&Source[sizeToMove]);
	
	if (strLength > 0)
	{
		memset(&Source[sizeToMove], '\0', strLength);
	}
	
}

/* ::::::::::::::: Initializer ::::::::::::::: */
void GSMCore::SetPowerGPIO(GPIO_TypeDef GPIOx, GPIOPin_TypeDef GPIO_Pin)
{
	this->_powerGPIOPin  = GPIO_Pin;
	this->_powerGPIOPort = GPIOx;
}

void GSMCore::Init()
{
}

/* ::::::::::::::::: Handle :::::::::::::::::: */
void GSMCore::UARTHandle(uint8_t *Data)
{
	
	if (this->_uartDataCounter < GSM_BUFFER_SIZE)
	{
		
		/* ~~~~~ Save data to receive buffer ~~~~~~ */
		this->_rBuff[this->_uartDataCounter] = *Data;
		this->_uartDataCounter++;
		
		/* ~~~~~~~~~~~~~ Reset timout ~~~~~~~~~~~~~ */
		this->_startTimeOut = GSM_GET_TICK();
		
		/* ~~~~~~~~~~~~~~~ Run event ~~~~~~~~~~~~~~ */
		(this->*DataHandle)();
		
	}
	
}

void GSMCore::IsReceived(char *cmd, GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~ Check response and run event ~~~~~ */
	if (strstr(this->_rBuff, cmd) != 0) // Response received
	{
		
		UserCallback(GSM_OK);
		
		/* ~~~~~~~~~~~~ Erase buffers ~~~~~~~~~~~~~ */
		this->ClearReceiveBuffer();

	}
	
}

void GSMCore::TimingHandle(void)
{
	
	if ((GSM_GET_TICK() - this->_startTimeOut) > this->_timeOut)
	{
		
		this->_startTimeOut = GSM_GET_TICK();
		this->_timeOut      = GSM_TIMOUT_DEF_MS;
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		if (this->_timeoutState == 1)
		{
			this->_timeoutState = 0;
			this->_userCallback(GSM_TIMOUT);
		}
		
		/* ~~~~~~~~~~~~ Erase buffers ~~~~~~~~~~~~~ */
		this->ClearReceiveBuffer();
		this->ClearTransmitBuffer();
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
	}
	
}

/* ::::::::::::::::: Timing :::::::::::::::::: */
void GSMCore::SetTimeOut(uint32_t Timout)
{
	
	this->_startTimeOut = GSM_GET_TICK();
	this->_timeOut      = Timout;
	this->_timeoutState = 1;
	
}

/* :::::::::::::: Data Transmit :::::::::::::: */
template <typename... Args>

void GSMCore::Transmit(Args... Data)
{
	
	/* ~~~~~~~~~~~~~ Format Data ~~~~~~~~~~~~~~ */
	sprintf(&this->_tBuff[this->_dataTransmitLength], Data...);
	
	/* ~~~~~~~~~~~~~ Transmit Data ~~~~~~~~~~~~ */
	GSM_UART_Transmit((uint8_t *)this->_tBuff, strlen(this->_tBuff));
	
// 	/* ~~~~~~ Clear the transmit buffer ~~~~~~~ */
// 	memset(this->_tBuff, '\0', strlen(this->_tBuff));
// 	
// 	this->_dataTransmitLength = 0;
	
}

/* ... AT Command ... */
template <typename... Args>

void GSMCore::TransmitAT(Args... CMD)
{
	
	/* ~~~~~~ Clear the transmit buffer ~~~~~~~ */
	memset(this->_tBuff, '\0', strlen(this->_tBuff));
	
	this->_dataTransmitLength = 0;
	
	/* ~~~~~ Write data to transmit buffer ~~~~ */
	GSMCore::WriteToTransmitBuffer((char *)"AT");
	
	/* ~~~~~~~ Add custom CMD to buffer ~~~~~~~ */
	GSMCore::Transmit(CMD...);
	
}

/* ~~~~~~~~~ Set response to check ~~~~~~~~ */
//strcpy(_cmdResp, "OK\r\n");

/* :::::::::::::: Modem Control :::::::::::::: */
/* ... Modem Power ... */
void GSMCore::PowerOn()
{
	GSM_GPIO_WritePin(this->_powerGPIOPort, (1 << this->_powerGPIOPin), GPIO_PIN_SET);
}

void GSMCore::PowerOff()
{
	GSM_GPIO_WritePin(this->_powerGPIOPort, (1 << this->_powerGPIOPin), GPIO_PIN_RESET);
}

/* ... Modem Echo ... */
void GSMCore::DisableEcho(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("E0\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

/* ... Modem Test ... */
void GSMCore::TestAT(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

/* ... Modem Info ... */
void GSMCore::GetModemInfo(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("I\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

char *GSMCore::ReadModemName()
{
	return (this->_rBuff);
}

/* ... GSM ... */
void GSMCore::SetPhoneFunctionality(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+CFUN=1,1\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

/* ... SIMCARD ... */
void GSMCore::SIMCARD_EnterPin(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+CPIN?\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

uint8_t GSMCore::SIMCARD_SIMStatus(void)
{
	
	if (strstr_P(this->_rBuff, GSM_RESP_ERROR) != 0)
	{
		return 0;		
	}
	else
	{
		return 1;
	}
	
}

/* ... SMS ... */
void GSMCore::SMS_Init(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+CMGF=1\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::SMS_InitEvent;
	
}

void GSMCore::SMS_InitHex(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+CMGF=1\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::SMS_InitHexEvent;
	
}

char *GSMCore::SMS_ReadData()
{	
	return (this->_rBuff);	
}

void GSMCore::SMSTo(GSM_CallbackTypeDef UserCallback, char *Number, char *Text)
{
	
	/* ~~~~~~ Clear the transmit buffer ~~~~~~~ */
	memset(this->_tBuff, '\0', strlen(this->_tBuff));
	memset(this->_smsBuff, '\0', strlen(this->_smsBuff));
	
	this->_dataTransmitLength = 0;
	
	/* ~~~~~ Write data to transmit buffer ~~~~ */
	GSMCore::WriteToTransmitBuffer((char *)"AT+CMGS=\"");
	GSMCore::WriteToTransmitBuffer(Number);
		
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::Transmit("\"\r\n");
	
	//this->_smsBuff = Text;
	strcpy(this->_smsBuff, Text);
	
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::SMS_SendEvent;
	
}

/* ... Call ... */
void GSMCore::Call_Init(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+CLIP=1\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

char *GSMCore::Call_ReadData()
{
	return (this->_rBuff);
}

void GSMCore::Call_Answer()
{
	GSMCore::Transmit("ATA\r\n");
}

void GSMCore::Call_Reject()
{
	GSMCore::Transmit("ATH\r\n");
}

void GSMCore::CallTo(GSM_CallbackTypeDef UserCallback, char *Number)
{
	
	/* ~~~~~~ Clear the transmit buffer ~~~~~~~ */
	memset(this->_tBuff, '\0', strlen(this->_tBuff));
	
	this->_dataTransmitLength = 0;
	
	/* ~~~~~ Write data to transmit buffer ~~~~ */
	GSMCore::WriteToTransmitBuffer((char *)"ATD+ ");
	GSMCore::WriteToTransmitBuffer(Number);
	//GSMCore::WriteToTransmitBuffer((char *)";\r\n");
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::Transmit(";\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::Call_StatusEvent;
	
}

/* ... Operator ... */
void GSMCore::GetOperatorInfo(GSM_CallbackTypeDef UserCallback)
{
	
	/* ~~~~~~ Transmit command to module ~~~~~~ */
	GSMCore::TransmitAT("+COPS?\r\n");
	GSMCore::SetTimeOut(GSM_TIMOUT_MS);
	
	/* ~~~~ Set event to run after response ~~~ */
	_userCallback = UserCallback;
	
	/* ~~~~ Set event to run after response ~~~ */
	this->DataHandle = &GSMCore::OKResponseEvent;
	
}

char *GSMCore::ReadOperatorCode()
{
	
// 	int8_t mode       = 0;
// 	int8_t format     = 0;
// 	char operCode[6]  = {0};
// 	
// 	sscanf(this->_rBuff, "\r\n+COPS: %d,%d,\"%[^\",]\"\r\n", &mode, &format, this->_rBuff);
	
//	return (this->_rBuff);
	
	int16_t strLength          = strlen(this->_rBuff);
	int16_t lastSpaceFromStart = 0;
	int16_t lastSpaceFromEnd   = strLength - 1;
	int16_t sizeToMove;
	
	while ((lastSpaceFromStart < strLength) && (this->_rBuff[lastSpaceFromStart] != '"'))
	{
		lastSpaceFromStart++;
	}
	
	lastSpaceFromStart++;
	
	while ((lastSpaceFromEnd > 0) && (this->_rBuff[lastSpaceFromEnd] != '"'))
	{
		lastSpaceFromEnd--;
	}
	
	sizeToMove = (lastSpaceFromEnd - lastSpaceFromStart);
	
	/* Shift correct data to first of array */
	if (sizeToMove > 0)
	{
		memmove(this->_rBuff, &this->_rBuff[lastSpaceFromStart], sizeToMove);
	}
	
	/* Erase remaining data */
	strLength = strlen(&this->_rBuff[sizeToMove]);
	
	if (strLength > 0)
	{
		memset(&this->_rBuff[sizeToMove], '\0', strLength);
	}
	
	return (this->_rBuff);
	
}

/* ::::::::::::::::: Events :::::::::::::::::: */
/* ... Global ... */
void GSMCore::NullEvent() {}

void GSMCore::OKResponseEvent()
{
	
	if (strstr_P(this->_rBuff, GSM_RESP_OK) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_OK_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_OK_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_OK);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_ERROR) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_ERROR_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_ERROR_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_ERROR);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	
}

/* ... SMS ... */
void GSMCore::SMS_InitEvent()
{
	
	static volatile uint8_t state = 0;
	
	if (strstr_P(this->_rBuff, GSM_RESP_OK) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Check State ~~~~~~~~~~~~~~ */
		if (state == 0)
		{
			
			state = 1;
			
			/* ------ Transmit command to module ------ */
			TransmitAT("+CNMI=1,2,0,0,0\r\n");
			SetTimeOut(GSM_TIMOUT_MS);
			
		}
		else
		{
			
			state = 0;
			
			/* ---------- Run user function ----------- */
			this->_userCallback(GSM_OK);
			
			/* ---------- Reset event to run ---------- */
			this->DataHandle = &GSMCore::NullEvent;
			
		}
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	
}

void GSMCore::SMS_InitHexEvent()
{
	
	static volatile uint8_t state = 0;
	
	if (strstr_P(this->_rBuff, GSM_RESP_OK) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Check State ~~~~~~~~~~~~~~ */
		if (state == 0)
		{
			
			state = 1;
			
			/* ------ Transmit command to module ------ */
			TransmitAT("+CNMI=1,2,0,0,0\r\n");
			SetTimeOut(GSM_TIMOUT_MS);
			
		}
		else if (state == 1)
		{
			
			state = 2;
			
			/* ------ Transmit command to module ------ */
			TransmitAT("+CSCS=\"HEX\"\r\n");
			SetTimeOut(GSM_TIMOUT_MS);
			
		}
		else if (state == 2)
		{
			
			state = 3;
			
			/* ------ Transmit command to module ------ */
			TransmitAT("+CSMP=49,167,0,8\r\n");
			SetTimeOut(GSM_TIMOUT_MS);
			
		}
		else if (state == 3)
		{
			
			state = 0;
			
			/* ---------- Run user function ----------- */
			this->_userCallback(GSM_OK);
			
			/* ---------- Reset event to run ---------- */
			this->DataHandle = &GSMCore::NullEvent;
			
		}
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	
}

void GSMCore::SMS_SendEvent()
{
	
	if (strstr_P(this->_rBuff, GSM_RESP_WAIT_TO_TEXT) != 0)
	{
		
		//GSM_UART_Transmit((uint8_t *)this->_smsBuff, strlen(this->_smsBuff));
		GSMCore::WriteToTransmitBuffer(this->_smsBuff);
		
		/* ~~~~~~ Transmit command to module ~~~~~~ */
		GSMCore::Transmit("\r\n%c", 26);
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_OK) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_OK_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_OK_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_OK);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_ERROR) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_ERROR_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_ERROR_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_ERROR);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	
}

/* ... Call ... */
void GSMCore::Call_StatusEvent()
{
	
	if (strstr_P(this->_rBuff, GSM_RESP_OK) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_OK_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_OK_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_OK);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_ERROR) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_ERROR_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_ERROR_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_ERROR);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_NO_CARRIER) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_NO_CARRIER_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_NO_CARRIER_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_CALL_NO_CARRIER);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	else if (strstr_P(this->_rBuff, GSM_RESP_BUSY) != 0)
	{
		
		/* ~~~~~~~~~~~~~ Modify Data ~~~~~~~~~~~~~~ */
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN_BUSY_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R_BUSY_R, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_RN, ' ');
		Replace(_rBuff, GSM_RESP_FOR_REMOVE_R, ' ');
		Trim(this->_rBuff);
		
		/* ~~~~~~~~~~ Run user function ~~~~~~~~~~~ */
		this->_userCallback(GSM_CALL_BUSY);
		
		/* ~~~~~~~~~~ Reset event to run ~~~~~~~~~~ */
		this->DataHandle = &GSMCore::NullEvent;
		
		/* ~~~~~~~~~ Clear receive buffer ~~~~~~~~~ */
		this->ClearReceiveBuffer();
		
	}
	
}

//String GetModemInfo();
//String GetModemName();

//bool FactoryDefault();
