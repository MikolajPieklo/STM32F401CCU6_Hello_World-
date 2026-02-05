/**
 ********************************************************************************
 * @file    runtime_stats.c
 * @author  Mikolaj Pieklo
 * @date    27.01.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void init_tim5(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void init_tim5(void)
{
   // if (1 == LL_TIM_IsEnabledCounter(TIM5))
   // {
   //    LL_TIM_DisableCounter(TIM5);
   //    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM5);
   // }

   /* Enable TIM5 clock */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);

   /* Configure TIM5 */
   LL_TIM_SetPrescaler(TIM5, 83999); /* Assuming 84MHz clock, prescaler to 1kHz */
   LL_TIM_SetCounterMode(TIM5, LL_TIM_COUNTERMODE_UP);
   LL_TIM_SetAutoReload(TIM5, 0xFFFFFFFF); /* Max ARR value */
   LL_TIM_EnableCounter(TIM5);
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void ConfigureTimerForRunTimeStats(void)
{
   init_tim5();
}

uint32_t GetRunTimeCounterValue(void)
{
   return LL_TIM_GetCounter(TIM5);
}
