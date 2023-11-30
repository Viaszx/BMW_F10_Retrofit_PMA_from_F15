/*
  **********************************************************************************
  * @title		can.c
  * @platform	STM32F107
  * @author		Anton Novikov
  * @version	V1.0.0
  * @date		13.02.2021
  *
  * @brief	 This code initializes CAN1 and CAN2 buses.
  *          for receiving messages on CAN1 and CAN2. It includes filter configuration for specific ID of BMW.
  *          and demonstrates handling received CAN messages, redirecting data from CAN1 to CAN2.
  **********************************************************************************
*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "misc.h"
#include "can.h"

const uint16_t CAN1_StdID = 0x050;		// Test CAN1
const uint16_t CAN2_StdID = 0x040;		// Test CAN2

int CAN1_Std = 0;
int CAN2_Std = 0;

const uint16_t CAN1_SurroundViewID = 0x387;			// BMW ID 5DL Surround_View
int ButtonPress = 0;

const uint16_t CAN2_SurroundViewID = 0x387;			// BMW ID 5DL Surround_View
uint8_t SurroundViewPack[2];

/**
  * @brief Configuring I/O ports, CAN1 and CAN2
  * @note This function is for configuring STM32F107 series microcontrollers.
  */
void init_CAN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* CAN GPIOs configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(CAN1_Periph | CAN2_Periph, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);

	// Set CAN1 RX pin
	GPIO_InitStructure.GPIO_Pin   = CAN1_RX_SOURCE;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN1_GPIO_PORT, &GPIO_InitStructure);

	// Set CAN1 TX pin
	GPIO_InitStructure.GPIO_Pin   = CAN1_TX_SOURCE;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	// Set CAN2 RX pin
	GPIO_InitStructure.GPIO_Pin   = CAN2_RX_SOURCE;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN2_GPIO_PORT, &GPIO_InitStructure);

	// Set CAN2 TX pin
	GPIO_InitStructure.GPIO_Pin   = CAN2_TX_SOURCE;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN2_GPIO_PORT, &GPIO_InitStructure);


	// Initialize CAN bus
	CAN_InitTypeDef CAN_InitStructure;

	CAN_DeInit( CAN1);
	CAN_DeInit( CAN2);

	CAN_StructInit(&CAN_InitStructure);

	// Initialization of CAN1 bus
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStructure.CAN_Prescaler = CAN1_SPEED_PRESCALE;
	CAN_Init(CAN1, &CAN_InitStructure);

	// Initialization of CAN2 bus
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStructure.CAN_Prescaler = CAN2_SPEED_PRESCALE;
	CAN_Init(CAN2, &CAN_InitStructure);

	// Configuring CAN filters
	// When configuring filters do not forget to pay attention to the filter numbers, otherwise you may overwrite them.
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	// Configuring filters for CAN1
	// Filter that skips all masked messages for a standard frame (32-bit scaling)
	CAN_FilterInitStructure.CAN_FilterNumber = 0;						// Filter number, available from 0 to 13
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	// Filter mode, Mask
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	// Bit rate (scaling)
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN1_StdID<<5;			// High part of the filter
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000; 					// Low part of the filter
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN1_StdID<<5; 		// Senior part of the mask
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000; 				// Low part of the mask
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; 		// FIFO buffer number (we have only two)
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 				// Filter activity
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN1_SurroundViewID << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN1_SurroundViewID << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// Configuring filters for CAN2
	// Filter that skips all masked messages for a standard frame (scaling by 32 bits)
	CAN_FilterInitStructure.CAN_FilterNumber = 14;						// Filter number, available from 14 to 27
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 	// Filter mode, Mask
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 	// Bit rate (scaling)
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN2_StdID<<5; 			// High part of the filter
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000; 					// Low part of the filter
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN2_StdID<<5; 		// Senior part of the mask
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000; 				// Low part of the mask
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0; 		// FIFO buffer number (we have only two)
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 				// Filter activity
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 15;
	CAN_FilterInitStructure.CAN_FilterIdHigh = CAN2_SurroundViewID << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN2_SurroundViewID << 5;
	CAN_FilterInit(&CAN_FilterInitStructure);

	// NVIC Configuration
	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// Interrupt setting for FIFO0 buffer processing CAN1
	// Enable CAN1 RX0 interrupt IRQ channel
	//NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// CAN1 FIFO0 message pending interrupt enable
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
/*
	// Interrupt setting for FIFO1 buffer processing CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// CAN1 FIFO1 message pending interrupt enable
	CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
*/
	//NVIC_InitTypeDef NVIC_InitStructure;
	// Interrupt setting for FIFO0 buffer processing CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	// CAN2 FIFO0 message pending interrupt enable
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
/*
	// Interrupt setting for FIFO1 buffer processing CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// CAN2 FIFO1 message pending interrupt enable
	//CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
*/

	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	//NVIC_EnableIRQ(CAN1_RX1_IRQn);
	NVIC_EnableIRQ(CAN2_RX0_IRQn);
	//NVIC_EnableIRQ(CAN2_RX1_IRQn);
	__enable_irq();
}

/**
  * @brief CAN1 bus interrupt handler for buffer FIFO0
  * @note None
  */
  
void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	// Initialize the values of RxMessage fields before receiving data.
	RxMessage.DLC = 	0x00;
	RxMessage.ExtId = 	0x00;
	RxMessage.FMI = 	0x00;
	RxMessage.IDE = 	0x00;
	RxMessage.RTR = 	0x00;
	RxMessage.StdId = 	0x00;
	RxMessage.Data [0] = 0x00;
	RxMessage.Data [1] = 0x00;
	RxMessage.Data [2] = 0x00;
	RxMessage.Data [3] = 0x00;
	RxMessage.Data [4] = 0x00;
	RxMessage.Data [5] = 0x00;
	RxMessage.Data [6] = 0x00;
	RxMessage.Data [7] = 0x00;

	// Check for the presence of CAN1 FIFO0 interrupt.
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
	{
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

		// Get actual reception of data from FIFO0.
		if (RxMessage.IDE == CAN_Id_Standard)
		{
			if (RxMessage.StdId == CAN1_StdID) {
				if (RxMessage.Data[0] == 0x01) {
				CAN1_Std = 1;
				CAN1_Test();
				} else {
				CAN1_Std = 0;
				}
			}
			if (RxMessage.StdId == CAN2_SurroundViewID) {
				SurroundViewPack[0] = RxMessage.Data[0];
				SurroundViewPack[1] = RxMessage.Data[1];
				}
			if (RxMessage.StdId == CAN1_SurroundViewID) {
				if (RxMessage.Data[0] == 0xFD) {
					ButtonPress = 1;
				} else {
					ButtonPress = 0;
				}
			}
		}
	}
}

/**
  * @brief CAN2 bus interrupt handler for buffer FIFO0
  * @note None
  */
  
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	RxMessage.DLC = 	0x00;
	RxMessage.ExtId = 	0x00;
	RxMessage.FMI = 	0x00;
	RxMessage.IDE = 	0x00;
	RxMessage.RTR = 	0x00;
	RxMessage.StdId = 	0x00;
	RxMessage.Data [0] = 0x00;
	RxMessage.Data [1] = 0x00;
	RxMessage.Data [2] = 0x00;
	RxMessage.Data [3] = 0x00;
	RxMessage.Data [4] = 0x00;
	RxMessage.Data [5] = 0x00;
	RxMessage.Data [6] = 0x00;
	RxMessage.Data [7] = 0x00;

	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);

		if (RxMessage.IDE == CAN_Id_Standard)
		{
			if (RxMessage.StdId == CAN2_StdID) {
				if (RxMessage.Data[0] == 0x01) {
				CAN2_Std = 1;
				CAN2_Test();
				} else {
				CAN2_Std = 0;
				}
			}
		}
	}
}

// Test message CAN1_Test
void CAN1_Test(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x123;
	TxMessage.ExtId = 0x00;

	TxMessage.IDE = CAN_Id_Standard;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8;

	TxMessage.Data[0] = 0x08;
	TxMessage.Data[1] = 0x07;
	TxMessage.Data[2] = 0x06;
	TxMessage.Data[3] = 0x05;
	TxMessage.Data[4] = 0x04;
	TxMessage.Data[5] = 0x03;
	TxMessage.Data[6] = 0x02;
	TxMessage.Data[7] = 0x01;

	CAN_Transmit(CAN1, &TxMessage);
}

// Test message CAN2_Test
void CAN2_Test(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x321;
	TxMessage.ExtId = 0x00;

	TxMessage.IDE = CAN_Id_Standard;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8;

	TxMessage.Data[0] = 0x01;
	TxMessage.Data[1] = 0x02;
	TxMessage.Data[2] = 0x03;
	TxMessage.Data[3] = 0x04;
	TxMessage.Data[4] = 0x05;
	TxMessage.Data[5] = 0x06;
	TxMessage.Data[6] = 0x07;
	TxMessage.Data[7] = 0x08;

	CAN_Transmit(CAN2, &TxMessage);
}

// Enable Surround View
void CAN2_Enable_Surround_View(void)
{
	CanTxMsg TxMessage;
	TxMessage.StdId = CAN2_SurroundViewID;
	TxMessage.ExtId = 0x00;

	TxMessage.IDE = CAN_Id_Standard;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 2;

	TxMessage.Data[0] = SurroundViewPack[0];
	TxMessage.Data[1] = SurroundViewPack[1];

// TxMessage.Data[2] = SurroundViewPack[2] | 0x0C;

	CAN_Transmit(CAN2, &TxMessage);
}
