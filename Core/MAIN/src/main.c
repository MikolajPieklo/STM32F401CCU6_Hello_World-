#include "main.h"

#include <stdint.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_crc.h>
#include <stm32f4xx_ll_spi.h>

#include <FreeRTOS.h>
#include <delay.h>
#include <device_info.h>
#include <gpio.h>
#include <task.h>

static void exampleTask(void *parameters) __attribute__((noreturn));

static void exampleTask(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   for (;;)
   {
      /* Example Task Code */
      LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      vTaskDelay(100); /* delay 100 ticks */
   }
}

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
#if !defined(USE_RTOS)
   LL_SYSTICK_EnableIT();
#endif

   /* Initialize all configured peripherals */
   MX_GPIO_Init();
   LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
   Device_Info();

#if defined(USE_RTOS)
   xTaskCreate(exampleTask, "example", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY,
               NULL);

   vTaskStartScheduler();
#endif
   while (1)
   {
      LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      TS_Delay_ms(100); /* delay 100 ticks */
   }
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
   /* USER CODE BEGIN 6 */
   /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
   /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#if defined(USE_RTOS)
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
    * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    * function that will get called if a call to pvPortMalloc() fails.
    * pvPortMalloc() is called internally by the kernel whenever a task, queue,
    * timer or semaphore is created.  It is also called by various parts of the
    * demo application.  If heap_1.c or heap_2.c are used, then the size of the
    * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    * to query the size of free heap space that remains (although it does not
    * provide information on how the remaining heap might be fragmented). */
   taskDISABLE_INTERRUPTS();

   for (;;)
   {
   }
}

void vApplicationIdleHook(void)
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    * task.  It is essential that code added to this hook function never attempts
    * to block in any way (for example, call xQueueReceive() with a block time
    * specified, or call vTaskDelay()).  If the application makes use of the
    * vTaskDelete() API function (as this demo application does) then it is also
    * important that vApplicationIdleHook() is permitted to return to its calling
    * function, because it is the responsibility of the idle task to clean up
    * memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
   (void) pcTaskName;
   (void) pxTask;

   /* Run time stack overflow checking is performed if
    * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    * function is called if a stack overflow is detected. */
   taskDISABLE_INTERRUPTS();

   for (;;)
   {
   }
}

void vApplicationTickHook(void)
{
}
#endif