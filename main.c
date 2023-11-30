/*
  **********************************************************************************
  * @title		main.c
  * @platform	STM32F107
  * @author		Anton Novikov
  * @version	V1.0.0
  * @date		13.02.2021
  *
  * @brief	 	Received CAN messages. It handles timer increments, checks button state.
  *          
  **********************************************************************************
*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "misc.h"
#include "can.h"

#include <stdio.h>
#include <stdbool.h>

void init_timer();
void res_timer(int timer_num, int timer_type);

typedef struct
  {
	bool run;	// Run timer
	bool res;	// Reset timer
	int val;	// Current timer value
  } tpTimer;

tpTimer timer_1000ms[10] = {{false, false, 0}};
int time_togle_1ms = 0;
int time_togle_1ms_old = 0;

tpTimer timer_10000x100ms[10] = {{false, false, 0}};
int time_togle_100ms = 0;
int time_togle_100ms_old = 0;

// ***************************
const int sendingPeriod = 1;				// Period sending time CAN message 100ms
// ***************************

// Current status of parcels
int CAN1_StdOn = 0;
int CAN2_StdOn = 0;

int main(void){

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	SystemInit();

	init_timer();

	init_CAN();

	while (1) {
		/* Timer management */
		// time_togle_1ms flag is toggled each 1ms
		if (time_togle_1ms != time_togle_1ms_old) {
			for (int i = 0; i < 10; i++) {
				if (timer_1000ms[i].run && timer_1000ms[i].val < 1000) {
					timer_1000ms[i].val++;
				}
			}
			time_togle_1ms_old = time_togle_1ms;
		}
		// time_togle_100ms flag is toggled each 100ms
		if (time_togle_100ms != time_togle_100ms_old) {
			for (int i = 0; i < 10; i++) {
				if (timer_10000x100ms[i].run
						&& timer_10000x100ms[i].val < 10000) {
					timer_10000x100ms[i].val++;
				}
			}
			time_togle_100ms_old = time_togle_100ms;
		}


		if (ButtonPress == 1) {
			timer_10000x100ms[1].run = true;
		}
		if (timer_10000x100ms[1].val >= sendingPeriod) {

			CAN2_Enable_Surround_View();
			timer_10000x100ms[1].run = false;
			res_timer(1, 100);
		}

		// Use the state set by the interrupt.
		CAN1_StdOn = CAN1_Std;
		CAN2_StdOn = CAN2_Std;
		// CAN1 ID 0x050
		if ((CAN1_Std != 0)) {
			GPIO_SetBits(GPIOB, GPIO_Pin_1);
		} else {
			GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		}
		// CAN2 ID 0x040
		if ((CAN2_Std != 0)) {
			GPIO_SetBits(GPIOB, GPIO_Pin_2);
		} else {
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
		}
	}
}

void res_timer(int timer_num, int timer_type) {
	if (timer_type == 1) {
		if (timer_num >= 0 && timer_num < 10) {
			timer_1000ms[timer_num].res = false;
			timer_1000ms[timer_num].val = 0;
		}
	} else if (timer_type == 100) {
		if (timer_num >= 0 && timer_num < 10) {
			timer_10000x100ms[timer_num].res = false;
			timer_10000x100ms[timer_num].val = 0;
		}
	}
}

void init_timer() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef base_timer;
	TIM_TimeBaseStructInit(&base_timer);

	base_timer.TIM_Prescaler = (SystemCoreClock / 10000) - 1;

	base_timer.TIM_Period = 10;
	base_timer.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &base_timer);

	base_timer.TIM_Period = 1000;
	base_timer.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &base_timer);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure3;
	NVIC_InitStructure3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure3);

	NVIC_InitTypeDef NVIC_InitStructure4;
	NVIC_InitStructure4.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure4);
}

void TIM3_IRQHandler() {

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		if (time_togle_1ms == 0) {
			time_togle_1ms = 1;
		} else {
			time_togle_1ms = 0;
		}
	}
}

void TIM4_IRQHandler() {

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		if (time_togle_100ms == 0) {
			time_togle_100ms = 1;
		} else {
			time_togle_100ms = 0;
		}
	}
}
