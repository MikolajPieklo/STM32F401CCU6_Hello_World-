/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include <stm32f4xx_ll_crc.h>
#include <stm32f4xx_ll_spi.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct program_info_typedef
{
   uint32_t text_sec_crc32;
   uint32_t data;
   uint32_t size;
} program_info_t;

__attribute__((section(".program_info_section"))) volatile const program_info_t program_info =
    {.text_sec_crc32 = 0xABCDEFAB, .data = 0x63, .size = 0xFFFFFFFF};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t systick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void System_Delay_ms(uint32_t ms);
void Configure_SPI(void);
void SPI_Tx8(uint8_t *data, uint8_t n);
void SPI_Rx8(uint8_t *data, uint8_t n);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_APB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);


    /* System interrupt init*/
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();
    LL_SYSTICK_EnableIT();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    /* USER CODE BEGIN 2 */
    LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
    // LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);
    Configure_SPI();
    /* USER CODE END 2 */

    System_Delay_ms(100);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

    LL_SPI_TransmitData8(SPI1, 0x00);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
    {
    }
    LL_SPI_TransmitData8(SPI1, 0x40);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
    {
    }
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

    System_Delay_ms(100);

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);

    LL_SPI_TransmitData8(SPI1, 0x40);
    while (!LL_SPI_IsActiveFlag_TXE(SPI1))
    {
    }
    uint8_t rx = LL_SPI_ReceiveData8(SPI1);
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
    {
    }
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

    if (rx != 0)
    {
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        if (program_info.data == 0x30)
        {
            LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        }

        System_Delay_ms(500);
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

void System_Delay_ms(uint32_t ms)
{
    uint32_t local_systick = systick;
    while (local_systick + ms > systick)
    {
    }
}

void Configure_SPI(void)
{
    /* (1) Enables GPIO clock and configures the SPI1 pins ********************/
    /* Enable the peripheral clock of GPIOA */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /* Configure CS Pin */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

    /* Configure SCK Pin */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_5, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_5, LL_GPIO_PULL_DOWN);

    /* Configure MISO Pin */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_DOWN);

    /* Configure MOSI Pin */
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);

    /* (2) Configure NVIC for SPI1 transfer complete/error interrupts **********/
    /* Set priority for SPI1_IRQn */
    NVIC_SetPriority(SPI1_IRQn, 0);
    /* Enable SPI1_IRQn           */
    NVIC_EnableIRQ(SPI1_IRQn);

    /* (3) Configure SPI1 functional parameters ********************************/

    /* Enable the peripheral clock of GPIOB */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    /* Configure SPI1 communication */
    LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV128);
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
    LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
    /* Reset value is LL_SPI_MSB_FIRST */
    // LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);
    LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

    /* Configure SPI1 transfer interrupts */
    /* Enable RXNE  Interrupt             */
    // LL_SPI_EnableIT_RXNE(SPI1);
    // /* Enable TXE   Interrupt             */
    // LL_SPI_EnableIT_TXE(SPI1);
    // /* Enable Error Interrupt             */
    // LL_SPI_EnableIT_ERR(SPI1);
    /* Enable SPI1 */
    LL_SPI_Enable(SPI1);
}
void SPI_Tx8(uint8_t *data, uint8_t n)
{
    uint8_t i = 0;

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    while (i < n)
    {
        LL_SPI_TransmitData8(SPI1, *(data + i));
        while (!LL_SPI_IsActiveFlag_TXE(SPI1))
        {
        }
        i++;
    }
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

void SPI_Rx8(uint8_t *data, uint8_t n)
{
    uint8_t i = 0;

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    while (i < n)
    {
        *(data + i) = LL_SPI_ReceiveData8(SPI1);
        while (!LL_SPI_IsActiveFlag_RXNE(SPI1))
        {
        }
        i++;
    }
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
}