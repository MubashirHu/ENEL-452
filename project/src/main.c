/**
 * @file main.c
 *
 * @brief Main Program for STM32F103RB Microcontroller.
 *
 * This file comprises the primary program logic that is intended to be flashed onto the STM32F103RB board.
 * It serves as the central component, orchestrating the microcontroller's operations and functions. 
 *
 * @author Mubashir Hussain
 * @studentID 200396797
 */

#include "../headers/USART.h"
#include "../headers/TIM.h"
#include "../headers/CLI.h"
#include "../headers/util.h"
#include "../headers/TASKS.h"
#include "../headers/i2c.h"
#include "../headers/i2c_lcd_driver.h"
#include "../headers/ELEVATOR.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f10x.h"

volatile uint8_t DATA_RECEIVED_FLAG = 0; 
volatile uint8_t TIM3_UPDATE_EVENT = 0;
volatile uint8_t TIM4_UPDATE_EVENT = 0;

extern QueueHandle_t xCLI_Queue;
extern uint8_t my_lcd_addr;

int main(void)
{	
	clockInit();
	serialOpen();
	prepareTerminal();
	initUSART2Interrupt();
	ledIOInit();
	i2c_init();
	lcd_init(my_lcd_addr);
	initTIM(2);
	initTIM(3);
	initTIM(4);
	configTIM(3, 1000);
	initTIMInterrupt(3);
	initTIMInterrupt(4);
	initGPIOPinsForElevator();
		
	createQueues();
	createTasks();
	
	vTaskStartScheduler();
}

void TIM3_IRQHandler(void) {
	TIM3_UPDATE_EVENT = 1;
	TIM3->SR &= ~(TIM_SR_UIF); // reset update event flag
}

void TIM4_IRQHandler(void) {
	TIM4_UPDATE_EVENT = 1;
	TIM4->SR &= ~(TIM_SR_UIF); // reset update event flag
}

void USART2_IRQHandler(void) {
	DATA_RECEIVED_FLAG = 1;
	USART2->SR &= ~(USART_SR_RXNE);	
	uint8_t characterReceived = USART2->DR;
	xQueueSendToFrontFromISR( xCLI_Queue, &characterReceived, NULL);
	DATA_RECEIVED_FLAG = 0;
}
