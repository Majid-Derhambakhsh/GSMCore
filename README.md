# GSMCore

"GSMCore" is a lightweight and portable embedded GSM modem driver for AT-command-based modules, supporting SMS messaging, voice calls, modem control, SIM card management, operator information retrieval, and UART communication.

## Release

### Version: 1.0.0

* #### Author: [Majid Derhambakhsh](https://github.com/Majid-Derhambakhsh)

* #### Type: Embedded Software.

* #### Support :

  ```
           - Any MCU
  ```

* #### Program Language: C++

* #### Requirement Libraries:

  ```
  ```

* #### Properties:

  ```
           - UART Based Communication
           - AT Command Support
           - GSM Modem Power Control
           - SMS Text Mode Support
           - SMS PDU/HEX Mode Support
           - Incoming SMS Detection
           - SMS Transmission
           - Voice Call Management
           - Incoming Call Detection
           - Call Answer / Reject
           - Outgoing Call Support
           - SIM Card Management
           - Operator Information Access
           - Modem Information Access
           - Callback Event System
           - Lightweight and Portable Design
  ```

* #### Changelog:

  ```
           - Initial Release
  ```

## Overview

### Types:

* `enum GSM_StatusTypeDef`: GSM Status, Params:

```c++
- GSM_ERROR
- GSM_OK
- GSM_TIMOUT
- GSM_CALL_NO_CARRIER
- GSM_CALL_BUSY
```

* `typedef void (*GSM_CallbackTypeDef)(GSM_StatusTypeDef Status)`: User Callback Function

```c++
void Callback(GSM_StatusTypeDef Status);
```

### Classes:

* `class GSMCore`

### Macros:

```c++
- None
```

### Buffer Functions:

```c++
void ClearReceiveBuffer(void);

void ClearTransmitBuffer(void);
```

### Initialization Functions:

```c++
void SetPowerGPIO(GPIO_TypeDef GPIOx,
                  GPIOPin_TypeDef GPIO_Pin);

void Init(void);
```

### Handle Functions:

```c++
void UARTHandle(uint8_t *Data);

void IsReceived(char *cmd,
                GSM_CallbackTypeDef UserCallback);

void TimingHandle(void);
```

### Data Transmit Functions:

```c++
template <typename... Args>
void Transmit(Args... Data);

template <typename... Args>
void TransmitAT(Args... CMD);
```

### Modem Control Functions:

```c++
void PowerOn(void);

void PowerOff(void);

void DisableEcho(GSM_CallbackTypeDef UserCallback);

void TestAT(GSM_CallbackTypeDef UserCallback);

void GetModemInfo(GSM_CallbackTypeDef UserCallback);

char *ReadModemName(void);

void SetPhoneFunctionality(GSM_CallbackTypeDef UserCallback);
```

### SIM Card Functions:

```c++
void SIMCARD_EnterPin(GSM_CallbackTypeDef UserCallback);

uint8_t SIMCARD_SIMStatus(void);
```

### SMS Functions:

```c++
void SMS_Init(GSM_CallbackTypeDef UserCallback);

void SMS_InitHex(GSM_CallbackTypeDef UserCallback);

char *SMS_ReadData(void);

void SMSTo(GSM_CallbackTypeDef UserCallback,
           char *Number,
           char *Text);
```

### Call Functions:

```c++
void Call_Init(GSM_CallbackTypeDef UserCallback);

char *Call_ReadData(void);

void Call_Answer(void);

void Call_Reject(void);

void CallTo(GSM_CallbackTypeDef UserCallback,
            char *Number);
```

### Operator Functions:

```c++
void GetOperatorInfo(GSM_CallbackTypeDef UserCallback);

char *ReadOperatorCode(void);
```

## Guide

#### This library can be used as follows:

#### 1. Add `GSMCore.hpp` header

```c++
#include "GSMCore.hpp"
```

#### 2. Define the `GSMCore` object in global section

```c++
GSMCore GSM;
```

#### 3. Configure modem power pin

```c++
GSM.SetPowerGPIO(GPIOA, GPIO_PIN_0);
```

#### 4. Initialize library

```c++
GSM.Init();
```

#### 5. Power on modem

```c++
GSM.PowerOn();
```

#### 6. Disable modem echo

```c++
void GSM_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {

    }
}

GSM.DisableEcho(GSM_Callback);
```

#### 7. Test modem communication

```c++
GSM.TestAT(GSM_Callback);
```

#### 8. Initialize SMS mode

```c++
GSM.SMS_Init(GSM_Callback);
```

#### 9. Send SMS

```c++
GSM.SMSTo(GSM_Callback,
          (char*)"+989123456789",
          (char*)"Hello World");
```

#### 10. Make a phone call

```c++
GSM.CallTo(GSM_Callback,
           (char*)"+989123456789");
```

#### 11. Handle received UART data

```c++
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    GSM.UARTHandle(&RxData);
}
```

#### 12. Execute timeout handler periodically

```c++
while(1)
{
    GSM.TimingHandle();
}
```

## Examples

#### Example 1: Send SMS via GSM Modem

```c++
#include "GSMCore.hpp"

GSMCore GSM;

volatile uint8_t GSMReady = 0;
volatile uint8_t SMSReady = 0;

void GSM_ModemFound_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        GSMReady = 1;
    }
}

void GSM_SMSInitialized_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        SMSReady = 1;
    }
}

void GSM_SMSSent_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("SMS Sent Successfully\r\n");
    }
    else
    {
        printf("SMS Send Failed\r\n");
    }
}

int main(void)
{
    HAL_Init();

    GSM.SetPowerGPIO(GPIOA,GPIO_PIN_0);

    GSM.Init();

    GSM.PowerOn();

    GSM.DisableEcho(nullptr);

    GSM.TestAT(GSM_ModemFound_Callback);

    GSM.SMS_Init(GSM_SMSInitialized_Callback);

    while(1)
    {
        GSM.TimingHandle();

        if((GSMReady == 1) &&
           (SMSReady == 1))
        {
            GSM.SMSTo(
                GSM_SMSSent_Callback,
                (char*)"+989123456789",
                (char*)"Hello World");

            break;
        }
    }
}
```

#### Example 2: Receive and Answer Incoming Calls

```c++
#include "GSMCore.hpp"

GSMCore GSM;

void GSM_CallInitialized_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Call Service Ready\r\n");
    }
}

void GSM_CallReceived_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Incoming Call\r\n");

        printf("Number : %s\r\n",
               GSM.Call_ReadData());

        GSM.Call_Answer();
    }
}

void GSM_CallEnded_Callback(GSM_StatusTypeDef Status)
{
    printf("Call Ended\r\n");
}

int main(void)
{
    HAL_Init();

    GSM.SetPowerGPIO(GPIOA,
                     GPIO_PIN_0);

    GSM.Init();

    GSM.PowerOn();

    GSM.Call_Init(
        GSM_CallInitialized_Callback);

    while(1)
    {
        GSM.TimingHandle();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t RxData;

    GSM.UARTHandle(&RxData);

    GSM.IsReceived(
        (char*)"RING",
        GSM_CallReceived_Callback);

    GSM.IsReceived(
        (char*)"NO CARRIER",
        GSM_CallEnded_Callback);
}

void UART_TxCpltCallback(void)
{
    GSM.ClearTransmitBuffer();
}
```

#### Example 3: Complete GSM Application

```c++
#include "GSMCore.hpp"

GSMCore GSM;

/* ---------------- Buffers ---------------- */

char SMSPhoneNumber[40];
char SMSDate[25];
char SMSText[100];

/* ---------------- GSM Callbacks ---------------- */

void GSM_EchoDisabled_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Echo Disabled\r\n");
    }
}

void GSM_ModemFound_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Modem Found\r\n");
    }
}

void GSM_ModemInfoReceived_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Modem : %s\r\n", GSM.ReadModemName());
    }
}

void GSM_SIMCARDStatusReceived_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("SIM Card Ready\r\n");
    }
    else
    {
        printf("SIM Card Error\r\n");
    }
}

void GSM_SMSInitialized_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("SMS Initialized\r\n");
    }
}

void GSM_CallInitialized_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Call Initialized\r\n");
    }
}

void GSM_OperatorInfoReceived_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Operator Code : %s\r\n", GSM.ReadOperatorCode());
    }
}

void GSM_PhoneFunctionality_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Phone Functionality Enabled\r\n");
    }
}

void GSM_SMSSent_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("SMS Sent\r\n");
    }
    else
    {
        printf("SMS Send Failed\r\n");
    }
}

void GSM_Dialing_Callback(GSM_StatusTypeDef Status)
{
    switch(Status)
    {
        case GSM_OK:
            printf("Call Connected\r\n");
            break;

        case GSM_CALL_BUSY:
            printf("Line Busy\r\n");
            break;

        case GSM_CALL_NO_CARRIER:
            printf("No Carrier\r\n");
            break;

        default:
            printf("Call Failed\r\n");
            break;
    }
}

/* ---------------- SMS Receive ---------------- */

void GSM_SMSReceived_Callback(GSM_StatusTypeDef Status)
{
    char PhoneBuff[40];
    char TextBuff[120];

    if(Status != GSM_OK)
    {
        return;
    }

    /*
        Example Received Data:

        +CMT: "+989123456789","","24/07/18,10:25:30+14"
        Hello World
    */

    sscanf(GSM.SMS_ReadData(),
           "\r\n+CMT: \"%[^\",]\",\"\",\"%20s\"\r\n%s\r\n"
           SMSPhoneNumber,
           SMSDate,
           SMSText);

    printf("SMS Received\r\n");
    printf("From : %s\r\n", SMSPhoneNumber);
    printf("Date : %s\r\n", SMSDate);
    printf("Text : %s\r\n", SMSText);

    /* Auto Reply */

    GSM.SMSTo(GSM_SMSSent_Callback,
              SMSPhoneNumber,
              (char*)"Message Received");

    /* Delete All SMS */

    GSM.TransmitAT("+CMGDA=\"DEL ALL\"\r\n");
}

/* ---------------- Call Receive ---------------- */

void GSM_CallReceived_Callback(GSM_StatusTypeDef Status)
{
    if(Status == GSM_OK)
    {
        printf("Incoming Call\r\n");
        printf("Number : %s\r\n", GSM.Call_ReadData());

        GSM.Call_Answer();
    }
}

void GSM_CallEnded_Callback(GSM_StatusTypeDef Status)
{
    printf("Call Ended\r\n");
}

/* ---------------- Main ---------------- */

int main(void)
{
    HAL_Init();

    GSM.SetPowerGPIO(GPIOA, GPIO_PIN_0);

    GSM.Init();

    GSM.PowerOn();

    GSM.DisableEcho(GSM_EchoDisabled_Callback);

    GSM.TestAT(GSM_ModemFound_Callback);

    GSM.GetModemInfo(GSM_ModemInfoReceived_Callback);

    GSM.SetPhoneFunctionality(GSM_PhoneFunctionality_Callback);

    GSM.SIMCARD_EnterPin(GSM_SIMCARDStatusReceived_Callback);

    GSM.SMS_InitHex(GSM_SMSInitialized_Callback);

    GSM.Call_Init(GSM_CallInitialized_Callback);

    GSM.GetOperatorInfo(GSM_OperatorInfoReceived_Callback);

    while(1)
    {
        GSM.TimingHandle();
    }
}

/* ---------------- UART Receive ---------------- */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t RxData;

    GSM.UARTHandle(&RxData);

    GSM.IsReceived((char*)"RING", GSM_CallReceived_Callback);

    GSM.IsReceived((char*)"23\r\n", GSM_SMSReceived_Callback);

    GSM.IsReceived((char*)"NO CARRIER", GSM_CallEnded_Callback);
}

/* ---------------- UART Transmit ---------------- */

void UART_TxCpltCallback(void)
{
    GSM.ClearTransmitBuffer();
}
```
## Test Performed:

#### MCU:
* [x] AVR
* [x] STM32

#### Modem:
* [x] SIM800C Series
* [ ] SIM900 Series

> [!NOTE]
> Be a part of making this library better by [sharing your thoughts and suggestions: majid.derhambakhsh@outlook.com](mailto:majid.derhambakhsh@outlook.com).
