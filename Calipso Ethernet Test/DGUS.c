#include "DGUS.h"
#include <string.h>
#include "Driver_USART.h"

#include "stm32f4xx_hal_uart.h"
#include <stdbool.h>

#ifdef USE_DGUS_DRIVER
ARM_DRIVER_USART* DGUS_USART_Driver;
#endif
extern osThreadId tid_MainThread;

UART_HandleTypeDef huart1;
uint8_t dgus_buffer_tx[BUFFER_NUM];
uint8_t dgus_buffer_rx[BUFFER_NUM];

unsigned char CRCTABH[256]={
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,
	0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,
	0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
	0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
	0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
	0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,
	0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,
	0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
	0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
	0x80,0x41,0x00,0xC1,0x81,0x40};
	
unsigned char CRCTABL[256]={
	0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,
	0x07,0xC7,0x05,0xC5,0xC4,0x04,0xCC,0x0C,0x0D,0xCD,
	0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
	0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,
	0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,0x14,0xD4,
	0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
	0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,
	0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
	0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,
	0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,
	0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,
	0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
	0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,
	0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,
	0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
	0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
	0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,
	0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
	0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,
	0x70,0xB0,0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,
	0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,
	0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,
	0x99,0x59,0x58,0x98,0x88,0x48,0x49,0x89,0x4B,0x8B,
	0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
	0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,
	0x43,0x83,0x41,0x81,0x80,0x40};

HAL_StatusTypeDef HAL_UART_Transmit_ITMY(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  if((pData == NULL ) || (Size == 0U)) 
  {
    return HAL_ERROR;
  }
  
  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->pTxBuffPtr = pData;
  huart->TxXferSize = Size;
  huart->TxXferCount = Size;

  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState = HAL_UART_STATE_BUSY_TX;

  /* Process Unlocked */
  __HAL_UNLOCK(huart);

  /* Enable the UART Transmit data register empty Interrupt */
  SET_BIT(huart->Instance->CR1, USART_CR1_TXEIE);
  
  return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Receive_ITMY(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  if((pData == NULL ) || (Size == 0U)) 
  {
    return HAL_ERROR;
  }
  
  /* Process Locked */
  __HAL_LOCK(huart);
  
  huart->pRxBuffPtr = pData;
  huart->RxXferSize = Size;
  huart->RxXferCount = Size;
  
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->RxState = HAL_UART_STATE_BUSY_RX;
  
  /* Process Unlocked */
  __HAL_UNLOCK(huart);
      
  /* Enable the UART Parity Error Interrupt */
  SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);
  
  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* Enable the UART Data Register not empty Interrupt */
  SET_BIT(huart->Instance->CR1, USART_CR1_RXNEIE);
  
  return HAL_OK;
}

/*uint16_t update_crc(uint16_t crc, char data)
{
	uint8_t crch, crcl, index;
	
	crcl = crc & 0xff;
	crch = (crc >> 8) & 0xff;
	
	index = crch ^ data;	
	crch  = crcl ^ CRCTABH[index];
	crcl  = CRCTABL[index];

	return (crch << 8) | crcl;
}*/

uint16_t
crc16_update(uint16_t crc, uint8_t a)
{
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }

    return crc;
}

uint16_t convert_w(uint16_t value)
{
	return (value >> 8) | (value << 8);
}

uint32_t convert_d(uint32_t value)
{
	return (value >> 24) || ((value & 0xff0000) >> 8) || ((value & 0xff00) << 8) || ((value & 0xff) << 24);
}

void convert_array_w(uint16_t* dst, uint16_t* src, uint16_t num)
{
	for (uint16_t i = 0; i < num/2; i++)
		dst[i] = convert_w(src[i]);
}

void WriteRegister(uint8_t addr, void *data, uint8_t num)
{
	DWIN_HEADERREG* header = (DWIN_HEADERREG*)dgus_buffer_tx;
	
	header->header = convert_w(HEADER_WORD);
	header->length = num + 2;
	header->cmd    = 0x80;
	header->addr   = addr;
	
#ifdef CRC_CHECK
	header->length = num + 4;
#endif
	
	memcpy(dgus_buffer_tx + 5, data, num);
	
#ifdef CRC_CHECK
	header->length = num + 4;
	uint16_t crc = 0xffff;
	for (uint16_t i = 3; i < num + 5; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[num + 5] = crc & 0xff;
	dgus_buffer_tx[num + 6] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 7);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 7);
#endif
#else
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 5);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 5);
#endif
#endif
}

void WriteVariable(uint16_t addr, void *data, uint8_t num)
{
	DWIN_HEADERDATA* header = (DWIN_HEADERDATA*)dgus_buffer_tx;
	
	header->header = convert_w(HEADER_WORD);
	header->length = num + 3;
	header->cmd    = 0x82;
	header->addr   = convert_w(addr);
	
#ifdef CRC_CHECK
	header->length = num + 5;
#endif
	
	memcpy(dgus_buffer_tx + 6, data, num);
	
#ifdef CRC_CHECK
	uint16_t crc = 0xffff;
	for (uint16_t i = 3; i < num + 6; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[num + 6] = crc & 0xff;
	dgus_buffer_tx[num + 7] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 8);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 8);
#endif
#else
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 6);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 6);
#endif
#endif
}

void WriteVariableConvert16(uint16_t addr, void *data, uint8_t num)
{
	DWIN_HEADERDATA* header = (DWIN_HEADERDATA*)dgus_buffer_tx;
	
	header->header = convert_w(HEADER_WORD);
	header->length = num + 3;
	header->cmd    = 0x82;
	header->addr   = convert_w(addr);

#ifdef CRC_CHECK
	header->length = num + 5;
#endif
	
	convert_array_w((uint16_t*)(dgus_buffer_tx + 6), (uint16_t*)data, num);
	
#ifdef CRC_CHECK
	uint16_t crc = 0xffff;
	for (uint16_t i = 3; i < num + 6; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[num + 6] = crc & 0xff;
	dgus_buffer_tx[num + 7] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 8);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 8);
#endif
#else
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 6);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 6);
#endif
#endif
}

void ReadRegister(uint8_t  addr, void **data, uint8_t num)
{
	DWIN_HEADERREG_REQ* header = (DWIN_HEADERREG_REQ*)dgus_buffer_tx;
	
	header->header = convert_w(HEADER_WORD);
	header->length = 3;
	header->cmd    = 0x81;
	header->addr   = addr;
	header->num    = num;
	
#ifdef CRC_CHECK
	header->length = 5;
	uint16_t crc = 0xFFFF;
	for (uint16_t i = 3; i < 6; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[6] = crc & 0xff;
	dgus_buffer_tx[7] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 6);
	DGUS_USART_Driver->Receive(dgus_buffer_rx, num + 6);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 6);
	HAL_UART_Receive_ITMY(&huart1, dgus_buffer_rx, num + 6);
#endif

#else
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 4);
	DGUS_USART_Driver->Receive(dgus_buffer_rx, num + 4);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 4);
	HAL_UART_Receive_ITMY(&huart1, dgus_buffer_rx, num + 4);
#endif
#endif
	*data = dgus_buffer_rx + 6;
}

void ReadVariable(uint16_t  addr, void **data, uint8_t num)
{
	DWIN_HEADERDATA_REQ* header = (DWIN_HEADERDATA_REQ*)dgus_buffer_tx;
	
	header->header = convert_w(HEADER_WORD);
	header->length = 4;
	header->cmd    = 0x83;
	header->addr   = convert_w(addr);
	header->num    = num/2;
	
#ifdef CRC_CHECK
	header->length = 6;
	uint16_t crc = 0xFFFF;
	for (uint16_t i = 3; i < 7; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[7] = crc & 0xff;
	dgus_buffer_tx[8] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, 9);
	DGUS_USART_Driver->Receive(dgus_buffer_rx, 9);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 9);
	HAL_UART_Receive_ITMY(&huart1, dgus_buffer_rx, num + 9);
#endif
	
	*data = dgus_buffer_rx + 7;
#else
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, 7);
	DGUS_USART_Driver->Receive(dgus_buffer_rx, num + 7);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 7);
	HAL_UART_Receive_ITMY(&huart1, dgus_buffer_rx, num + 7);
#endif
	
	*data = dgus_buffer_rx + 7;
#endif
}

uint16_t GetPicId(uint32_t timeout, uint16_t pic_id)
{
	uint16_t* pvalue;
	ReadRegister(REGISTER_ADDR_PICID, (void**)&pvalue, 2);
	
	if (osSignalWait(DGUS_EVENT_SEND_COMPLETED, timeout).status != osEventTimeout)
		if (osSignalWait(DGUS_EVENT_RECEIVE_COMPLETED, timeout).status != osEventTimeout)
			return convert_w(*pvalue);
		
	return pic_id;
}

void SetPicId(uint16_t pic_id, uint16_t timeout)
{
	uint16_t value = convert_w(pic_id);
	WriteRegister(REGISTER_ADDR_PICID, &value, sizeof(value));
	osSignalWait(DGUS_EVENT_SEND_COMPLETED, timeout);
}

/* Private functions ---------------------------------------------------------*/
void DWIN_USART_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_USART_EVENT_RECEIVE_COMPLETE:  
				/* Success: Wakeup Thread */
				osSignalSet(tid_MainThread, DGUS_EVENT_RECEIVE_COMPLETED);
        break;
    case ARM_USART_EVENT_TRANSFER_COMPLETE:
				/* Success: Wakeup Thread */
				break;
    case ARM_USART_EVENT_SEND_COMPLETE:
				/* Success: Wakeup Thread */
				osSignalSet(tid_MainThread, DGUS_EVENT_SEND_COMPLETED);
				break;
    case ARM_USART_EVENT_TX_COMPLETE:
        /* Success: Wakeup Thread */
        break;
 
    case ARM_USART_EVENT_RX_TIMEOUT:
         //__breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
 
    case ARM_USART_EVENT_RX_OVERFLOW:
    case ARM_USART_EVENT_TX_UNDERFLOW:
        //__breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	osSignalSet(tid_MainThread, DGUS_EVENT_RECEIVE_COMPLETED);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	osSignalSet(tid_MainThread, DGUS_EVENT_SEND_COMPLETED);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	huart->ErrorCode = HAL_UART_ERROR_NONE;
}

/* *************************************** Helper Laser Diode Data Functions ************************** */

void convert_laserdata(DGUS_LASERDIODE* dst, DGUS_LASERDIODE* src)
{
	dst->state								  = convert_w(src->state);
	dst->mode                   = convert_w(src->mode);
	convert_array_w((uint16_t*)&dst->laserprofile, (uint16_t*)&src->laserprofile, sizeof(DGUS_LASERPROFILE));
	convert_array_w((uint16_t*)&dst->lasersettings, (uint16_t*)&src->lasersettings, sizeof(DGUS_LASERSETTINGS));
	dst->PulseCounter           = convert_d(src->PulseCounter);
	dst->melanin                = convert_w(src->melanin);
	dst->phototype              = convert_w(src->phototype);
	dst->temperature            = convert_w(src->temperature);
	dst->cooling                = convert_w(src->cooling);
	dst->flow                   = convert_w(src->flow);
	convert_array_w((uint16_t*)&dst->timer, (uint16_t*)&src->timer, sizeof(DGUS_PREPARETIMER));
	dst->coolIcon								= convert_w(src->coolIcon);
	dst->SessionPulseCounter    = convert_d(src->SessionPulseCounter);
	convert_array_w((uint16_t*)&dst->buttons, (uint16_t*)&src->buttons, sizeof(DGUS_LASERDIODE_CONTROLBTN));
}

void convert_laserdata_ss(DGUS_SOLIDSTATELASER* dst, DGUS_SOLIDSTATELASER* src)
{
	dst->state								  = convert_w(src->state);
	dst->mode                   = convert_w(src->mode);
	convert_array_w((uint16_t*)&dst->laserprofile, (uint16_t*)&src->laserprofile, sizeof(DGUS_LASERPROFILE));
	convert_array_w((uint16_t*)&dst->lasersettings, (uint16_t*)&src->lasersettings, sizeof(DGUS_LASERSETTINGS));
	dst->PulseCounter           = convert_d(src->PulseCounter);
	dst->SessionPulseCounter    = convert_d(src->SessionPulseCounter);
	convert_array_w((uint16_t*)&dst->buttons, (uint16_t*)&src->buttons, sizeof(DGUS_SOLIDSTATELASER_CONTROLBTN));
	dst->connector              = convert_w(src->connector);
}

void WriteLaserDiodeDataConvert16(uint16_t addr, DGUS_LASERDIODE *data)
{
	DWIN_HEADERDATA* header = (DWIN_HEADERDATA*)dgus_buffer_tx;
	
	uint16_t num = sizeof(DGUS_LASERDIODE);
	
	header->header = convert_w(HEADER_WORD);
	header->length = num + 3;
	header->cmd    = 0x82;
	header->addr   = convert_w(addr);
	
#ifdef CRC_CHECK
	header->length = num + 5;
#endif
	
	convert_laserdata((DGUS_LASERDIODE*)(dgus_buffer_tx + 6), data);
	
#ifdef CRC_CHECK
	uint16_t crc = 0xffff;
	for (uint16_t i = 3; i < num + 6; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[num + 6] = crc & 0xff;
	dgus_buffer_tx[num + 7] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 8);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 8);
#endif
#else
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 6);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 6);
#endif
#endif
}

void WriteSolidStateLaserDataConvert16(uint16_t addr, DGUS_SOLIDSTATELASER *data)
{
	DWIN_HEADERDATA* header = (DWIN_HEADERDATA*)dgus_buffer_tx;
	
	uint16_t num = sizeof(DGUS_SOLIDSTATELASER);
	
	header->header = convert_w(HEADER_WORD);
	header->length = num + 3;
	header->cmd    = 0x82;
	header->addr   = convert_w(addr);
	
#ifdef CRC_CHECK
	header->length = num + 5;
#endif
	
	convert_laserdata_ss((DGUS_SOLIDSTATELASER*)(dgus_buffer_tx + 6), data);
	
#ifdef CRC_CHECK
	uint16_t crc = 0xffff;
	for (uint16_t i = 3; i < num + 6; i++)
		crc = crc16_update(crc, dgus_buffer_tx[i]); //_crc16_update
	dgus_buffer_tx[num + 6] = crc & 0xff;
	dgus_buffer_tx[num + 7] = crc >> 8;
	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 8);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 8);
#endif
#else	
	osSignalClear(tid_MainThread, 0);
#ifdef USE_DGUS_DRIVER
	DGUS_USART_Driver->Send(dgus_buffer_tx, num + 6);
#else
	HAL_UART_Transmit_ITMY(&huart1, dgus_buffer_tx, num + 6);
#endif
#endif
}

void Initialize_DGUS()
{
	__USART1_CLK_ENABLE();
	
	UART_InitTypeDef init_uart = {0};
	
	init_uart.BaudRate = 115200;
  init_uart.WordLength = UART_WORDLENGTH_8B;
  init_uart.StopBits = UART_STOPBITS_1;
  init_uart.Parity = UART_PARITY_NONE;
  init_uart.Mode = UART_MODE_TX_RX;
  init_uart.HwFlowCtl = UART_HWCONTROL_NONE;
  init_uart.OverSampling = UART_OVERSAMPLING_16;
	
	huart1.Init = init_uart;
	huart1.Instance = USART1;
	
	HAL_UART_Init(&huart1);
	
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}
