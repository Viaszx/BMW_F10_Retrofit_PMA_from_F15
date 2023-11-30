/*
  **********************************************************************************
  * @title		can.h
  * @platform	STM32F107
  * @author		Anton Novikov
  * @version	V1.0.0
  * @date		13.02.2021
  *
  * @brief		Header file for CAN communication configuration.
  *
  **********************************************************************************
*/

#ifndef __CAN
#define __CAN

//#define CAN1_ReMap
// CAN1 MASTER
#ifndef CAN1_ReMap
	#define CAN1_GPIO_PORT			GPIOB
	#define CAN1_RX_SOURCE			GPIO_Pin_8
	#define CAN1_TX_SOURCE			GPIO_Pin_9
	#define CAN1_Periph				RCC_APB2Periph_GPIOB
#else
	#define CAN1_GPIO_PORT			GPIOA
	#define CAN1_RX_SOURCE			GPIO_Pin_11
	#define CAN1_TX_SOURCE			GPIO_Pin_12
	#define CAN1_Periph				RCC_APB2Periph_GPIOA
#endif

//#define CAN2_ReMap
// CAN2 SLAVE
#ifndef CAN2_ReMap
	#define CAN2_GPIO_PORT			GPIOB
	#define CAN2_RX_SOURCE			GPIO_Pin_12
	#define CAN2_TX_SOURCE			GPIO_Pin_13
	#define CAN2_Periph				RCC_APB2Periph_GPIOB
#else
	#define CAN1_GPIO_PORT			GPIOB
	#define CAN1_RX_SOURCE			GPIO_Pin_5
	#define CAN1_TX_SOURCE			GPIO_Pin_6
	#define CAN1_Periph				RCC_APB2Periph_GPIOB
#endif

 #define CAN1_SPEED_PRESCALE			45
 #define CAN2_SPEED_PRESCALE			9

// Identifier Extension Bit
#define CAN_IDE_32			0b00000100
#define CAN_IDE_16			0b00001000
// Remote Transmission Request
#define CAN_RTR_32			0b00000010
#define CAN_RTR_16			0b00010000


void init_CAN(void);
void CAN1_RX0_IRQHandler(void);
void CAN2_RX0_IRQHandler(void);

void CAN1_Test(void);
void CAN2_Test(void);
void CAN2_Enable_Surround_View(void);

extern int CAN1_Std;
extern int CAN2_Std;
extern int ButtonPress;

#endif //__CAN
