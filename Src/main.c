/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "App_main.h"
#include "glcd.h"
#include "cmd_a.h"

#include "dev_INA219.h"
#include "exsram.h"
#include "TestFuncs.h"

#include "FreeRTOS.h"
#include "semphr.h"

//#include "core_cm4.h"

extern xSysState SysStates;
extern SemaphoreHandle_t xMutexHandleCriticalAssertProtection;		//用于打印的临界保护
void delay_us(uint32_t t);

#include "TestFuncs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char rtc_tab[7];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_CRC_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_FSMC_Init();
  MX_RTC_Init();
  MX_UART4_Init();
  MX_USART6_UART_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */


  ina219_init(1);	//1号表用来记录数据
  ina219_init(2);	//2号表只用来检测是否短路

// need to add
  //5=sck,7=datain,9=dataout,11=cs
    HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	GLCD_wait_power_on();
	printf("GLCD init done.\n");
	GLCD_system_reset();
	GLCD_bfcolor_set(1, 0xffffff, 0x000000);
	GLCD_trcolor_set(1, 0xf0f0f0);
	GLCD_fontlib_set(1, 8, 16, "a16:u16");
	CMDA_backlight_setting(12);
//	rtc_tab[6]=19; rtc_tab[5]=8; rtc_tab[3]=1; rtc_tab[2]=23; rtc_tab[1]=58; rtc_tab[0]=55; rtc_tab[4]=3;
//	CMDA_rtc_setting(&rtc_tab[0]);
//	CMDA_rtc_display(0xffffff, 800-26*8-4, 4, 1, 1);
//	CMDA_buzzer_beep(60);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

	//setting current amter
	CurrentAmterContinousOn();				//上电后直接开启，数据选测性接收
//	CurrentAmterContinousOff();
//	HAL_UART_Receive_DMA(&CurrentPort,(uint8_t *)(0x68000000),4096);
//	HAL_UART_AbortReceive_IT(&CurrentPort);

    App_main();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void delay_us(uint32_t t)
{
#define numof1us	42
	uint32_t temp=t;
	while(temp--)
	{
		for( uint32_t i=numof1us;i;i--);
	}
}

uint32_t SetCalendar(xCalendar *pCalendar)
{
	HAL_RTC_SetTime(&hrtc,&pCalendar->htime,RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc,&pCalendar->hdate,RTC_FORMAT_BIN);

//	HAL_RTC_GetTime(&hrtc,&pCalendar->htime,RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&hrtc,&pCalendar->hdate,RTC_FORMAT_BIN);

	return 0;
}

uint32_t GetCalendar(xCalendar *pCalendar)
{
	HAL_RTC_GetTime(&hrtc,&pCalendar->htime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&pCalendar->hdate,RTC_FORMAT_BIN);

	return 0;
}

void HostBeep(uint32_t state)
{
	HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,state);
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	printf("low level init error\r\n");
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	//如果xMutexHandleCriticalAssertProtection为NULL，则会进入configasser -> while(1)
	if(SysStates.osstates)						//OS has started，use os assert
	{
		/* See if the mutex can be obtained. If the mutex is not available
		wait 10 ticks to see if it becomes free. */
		if( xSemaphoreTake( xMutexHandleCriticalAssertProtection, 30*1000 ) == pdTRUE )		//max overtime:30 seconds
		{
			/* The mutex was successfully obtained so the shared resource can be
			accessed safely. */
			if(line)
				printf("Assert_failed: %s : %d\r\n", file, (int)line);
			else
				printf("%s", file);
			/* Access to the shared resource is complete, so the mutex is
			returned. */
			xSemaphoreGive( xMutexHandleCriticalAssertProtection );
		}
		else
		{
			/* The mutex could not be obtained even after waiting 10 ticks, so
			the shared resource cannot be accessed. */
			printf("Assert itself OverTime\r\n");
		}
	}
	else														//standalone mode ,os don't start
	{
		if(line)
			printf("Assert_failed: %s : %d\r\n", file, (int)line);
		else
			printf("%s", file);
	}

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
