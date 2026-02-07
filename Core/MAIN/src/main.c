#include "main.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_crc.h>
#include <stm32f4xx_ll_spi.h>

#ifdef USED_RTOS
#include <FreeRTOS.h>
#include <task.h>
#include <task_monitor.h>
#endif
#include <circual_buffer.h>
#include <delay.h>
#include <device_info.h>
#include <gpio.h>
#include <log.h>
#include <reuse.h>
#include <rtc.h>
#include <spi.h>
#include <string.h>
#include <uart.h>

/* Dummy device */
static const struct device main_dev = {
   .name = "MAIN",
};

CirBuff_T cb_uart1_tx = {.tail = 0, .head = 0, .USARTx = USART1};
CirBuff_T cb_uart1_rx = {.tail = 0, .head = 0, .USARTx = USART1};

#ifdef USED_RTOS
static void exampleTask(void *parameters) __attribute__((noreturn));
static void exampleTask1(void *parameters) __attribute__((noreturn));

static void exampleTask(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   for (;;)
   {
      /* Example Task Code */
      log_info(&main_dev, "Test A\r\n");
      LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      vTaskDelay(5000); /* delay 1000 ticks */
   }
}

static void exampleTask1(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   for (;;)
   {
      log_info(&main_dev, "Task B\r\n");
      vTaskDelay(5500); /* delay 1000 ticks */
   }
}
#endif

void SystemClock_Config(void);
void System_Delay_ms(uint32_t ms);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
   /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   /* System interrupt init*/
   NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   /* SysTick_IRQn interrupt configuration */
   NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

   /* Configure the system clock */
   SystemClock_Config();
#ifndef USED_RTOS
   LL_SYSTICK_EnableIT();
#endif
   UART1_Init();

   /* Initialize all configured peripherals */
   MX_GPIO_Init();
   LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
   RTC_Init();
   Device_Info();
   SPI1_Init();

#ifdef USED_RTOS
   xTaskCreate(exampleTask, "example", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY,
               NULL);
   xTaskCreate(exampleTask1, "example1", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY,
               NULL);
   ConfigureTimerForRunTimeStats();
   StartTaskMonitor();

   vTaskStartScheduler();
#endif
   while (1)
   {
#ifndef USED_RTOS
      LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      TS_Delay_ms(1000); /* delay 1000 ticks */
      log_info(&main_dev, "Hello\r\n");
#endif
   }

   return 0;
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
   while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
   {
   }
   LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
   LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
   while (LL_RCC_HSE_IsReady() != 1)
   {
   }
   LL_RCC_HSE_EnableCSS();
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 168, LL_RCC_PLLP_DIV_2);
   LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
   while (LL_RCC_PLL_IsReady() != 1)
   {
   }
   while (LL_PWR_IsActiveFlag_VOS() == 0)
   {
   }
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   }
   LL_Init1msTick(84000000);
   LL_SetSystemCoreClock(84000000);
   LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
   /* USER CODE BEGIN Error_Handler_Debug */
   /* User can add his own implementation to report the HAL error return state */
   __disable_irq();
   while (1)
   {
   }
   /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
   REUSE_UNUSED(file);
   REUSE_UNUSED(line);
   /* USER CODE BEGIN 6 */
   /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
   /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
