/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "mk_dht11.h"
#include "rtc.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stdio.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
dht11_t dht11;
xLogData_t xLogData;

GPIO_TypeDef *xRelayPorts[5] =
  {LD2_GPIO_Port, RELAY1_GPIO_Port, RELAY2_GPIO_Port, RELAY3_GPIO_Port, RELAY4_GPIO_Port};
uint16_t uiRelayPins[5] = {LD2_Pin, RELAY1_Pin, RELAY2_Pin, RELAY3_Pin, RELAY4_Pin};

/* USER CODE END Variables */
/* Definitions for transmitUART */
osThreadId_t transmitUARTHandle;
const osThreadAttr_t transmitUART_attributes = {
  .name = "transmitUART",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 256 * 4
};
/* Definitions for updateDisplay */
osThreadId_t updateDisplayHandle;
const osThreadAttr_t updateDisplay_attributes = {
  .name = "updateDisplay",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for readSensorsRTC */
osThreadId_t readSensorsRTCHandle;
const osThreadAttr_t readSensorsRTC_attributes = {
  .name = "readSensorsRTC",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for controlWatering */
osThreadId_t controlWateringHandle;
const osThreadAttr_t controlWatering_attributes = {
  .name = "controlWatering",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for updateLog */
osThreadId_t updateLogHandle;
const osThreadAttr_t updateLog_attributes = {
  .name = "updateLog",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for cfgWatering */
osThreadId_t cfgWateringHandle;
const osThreadAttr_t cfgWatering_attributes = {
  .name = "cfgWatering",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for queueReturnUART */
osMessageQueueId_t queueReturnUARTHandle;
const osMessageQueueAttr_t queueReturnUART_attributes = {
  .name = "queueReturnUART"
};
/* Definitions for queueConfigureWateringCommand */
osMessageQueueId_t queueConfigureWateringCommandHandle;
const osMessageQueueAttr_t queueConfigureWateringCommand_attributes = {
  .name = "queueConfigureWateringCommand"
};
/* Definitions for queueConfigureWateringControl */
osMessageQueueId_t queueConfigureWateringControlHandle;
const osMessageQueueAttr_t queueConfigureWateringControl_attributes = {
  .name = "queueConfigureWateringControl"
};
/* Definitions for mutexLogData */
osMutexId_t mutexLogDataHandle;
osStaticMutexDef_t mutexLogDataControlBlock;
const osMutexAttr_t mutexLogData_attributes = {
  .name = "mutexLogData",
  .cb_mem = &mutexLogDataControlBlock,
  .cb_size = sizeof(mutexLogDataControlBlock),
};
/* Definitions for mutexFLASH */
osMutexId_t mutexFLASHHandle;
osStaticMutexDef_t mutexFLASHControlBlock;
const osMutexAttr_t mutexFLASH_attributes = {
  .name = "mutexFLASH",
  .cb_mem = &mutexFLASHControlBlock,
  .cb_size = sizeof(mutexFLASHControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void ssd1306_WriteTemp(uint8_t temp, uint8_t x, uint8_t y, SSD1306_COLOR color);
/* USER CODE END FunctionPrototypes */

void transmitUARTTask(void *argument);
void updateDisplayTask(void *argument);
void readSensorsRTCTask(void *argument);
void controlWateringTask(void *argument);
void updateLogTask(void *argument);
void cfgWateringTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of mutexLogData */
  mutexLogDataHandle = osMutexNew(&mutexLogData_attributes);

  /* creation of mutexFLASH */
  mutexFLASHHandle = osMutexNew(&mutexFLASH_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of queueReturnUART */
  queueReturnUARTHandle = osMessageQueueNew (16, sizeof(uint8_t), &queueReturnUART_attributes);

  /* creation of queueConfigureWateringCommand */
  queueConfigureWateringCommandHandle = osMessageQueueNew (5, sizeof(uint16_t), &queueConfigureWateringCommand_attributes);

  /* creation of queueConfigureWateringControl */
  queueConfigureWateringControlHandle = osMessageQueueNew (5, sizeof(uint16_t), &queueConfigureWateringControl_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of transmitUART */
  transmitUARTHandle = osThreadNew(transmitUARTTask, NULL, &transmitUART_attributes);

  /* creation of updateDisplay */
  updateDisplayHandle = osThreadNew(updateDisplayTask, NULL, &updateDisplay_attributes);

  /* creation of readSensorsRTC */
  readSensorsRTCHandle = osThreadNew(readSensorsRTCTask, NULL, &readSensorsRTC_attributes);

  /* creation of controlWatering */
  controlWateringHandle = osThreadNew(controlWateringTask, NULL, &controlWatering_attributes);

  /* creation of updateLog */
  updateLogHandle = osThreadNew(updateLogTask, NULL, &updateLog_attributes);

  /* creation of cfgWatering */
  cfgWateringHandle = osThreadNew(cfgWateringTask, NULL, &cfgWatering_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_transmitUARTTask */
/**
  * @brief  Function implementing the transmitUART thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_transmitUARTTask */
void transmitUARTTask(void *argument)
{
  /* USER CODE BEGIN transmitUARTTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END transmitUARTTask */
}

/* USER CODE BEGIN Header_updateDisplayTask */
/**
* @brief Function implementing the updateDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_updateDisplayTask */
void updateDisplayTask(void *argument)
{
  /* USER CODE BEGIN updateDisplayTask */
  char cDate[9], cTime[9], cHum[4], cSoil[4][4];
  uint8_t uiRelaySelected, uiRelayStatus;

  ssd1306_Init();
  ssd1306_FillRectangle(0, 0, 127, 15, White);

  ssd1306_SetCursor(0, 16);
  ssd1306_WriteString("CH1", Font_7x10, White);
  ssd1306_SetCursor(0, 27);
  ssd1306_WriteString("68%", Font_7x10, White);
  ssd1306_DrawRectangle( 23, 16, 34, 36, White);

  ssd1306_SetCursor(38, 16);
  ssd1306_WriteString("CH2", Font_7x10, White);
  ssd1306_SetCursor(38, 27);
  ssd1306_WriteString("52%", Font_7x10, White);
  ssd1306_DrawRectangle( 61, 16, 72, 36, White);

  ssd1306_SetCursor(0, 41);
  ssd1306_WriteString("CH3", Font_7x10, White);
  ssd1306_SetCursor(0, 52);
  ssd1306_WriteString("00%", Font_7x10, White);
  ssd1306_DrawRectangle( 23, 41, 34, 61, White);

  ssd1306_SetCursor(38, 41);
  ssd1306_WriteString("CH4", Font_7x10, White);
  ssd1306_SetCursor(38, 52);
  ssd1306_WriteString("99%", Font_7x10, White);
  ssd1306_DrawRectangle( 61, 41, 72, 61, White);

  ssd1306_SetCursor(74, 16);
  ssd1306_WriteString(" Ambient ", Font_6x8, Black);

  ssd1306_UpdateScreen();
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
    if(osMutexAcquire(mutexLogDataHandle, osWaitForever) == osOK)
    {
    	sprintf(cDate, "%02d/%02d/%02d", xLogData.s.day, xLogData.s.month, xLogData.s.year);
    	sprintf(cTime, "%02d:%02d:%02d", xLogData.s.hour, xLogData.s.minute, xLogData.s.second);

    	sprintf(cHum, "%d%%", xLogData.s.ambHumidity);
    	ssd1306_WriteTemp(xLogData.s.ambTemperature, 84, 46, White);

    	sprintf(cSoil[0], "%d%%", xLogData.s.soilHumidity[0]);
    	sprintf(cSoil[1], "%d%%", xLogData.s.soilHumidity[1]);
    	sprintf(cSoil[2], "%d%%", xLogData.s.soilHumidity[2]);
    	sprintf(cSoil[3], "%d%%", xLogData.s.soilHumidity[3]);

    	uiRelaySelected = xLogData.s.relaySelected;
    	uiRelayStatus = xLogData.s.relayStatus;

    	osMutexRelease(mutexLogDataHandle);

    	ssd1306_SetCursor(2, 3);
    	ssd1306_WriteString(cDate, Font_7x10, Black);

    	ssd1306_SetCursor(70, 3);
    	ssd1306_WriteString(cTime, Font_7x10, Black);

    	ssd1306_SetCursor(84, 28);
    	ssd1306_WriteString(cHum, Font_11x18, White);

    	ssd1306_FillRectangle( 24, 17, 33, 35, 0x01 &  uiRelayStatus);
    	ssd1306_FillRectangle( 62, 17, 71, 35, 0x01 & (uiRelayStatus >> 1));
    	ssd1306_FillRectangle( 24, 42, 33, 60, 0x01 & (uiRelayStatus >> 2));
    	ssd1306_FillRectangle( 62, 42, 71, 60, 0x01 & (uiRelayStatus >> 3));

    	switch(uiRelaySelected)
		{
    		case 0x01:
			  ssd1306_Line(0, 38, 34, 38, White);
			  ssd1306_Line(38, 38, 72, 38, Black);
			  ssd1306_Line(0, 63, 34, 63, Black);
			  ssd1306_Line(38, 63, 72, 63, Black);

			  break;
    		case 0x02:
			  ssd1306_Line(0, 38, 34, 38, Black);
			  ssd1306_Line(38, 38, 72, 38, White);
			  ssd1306_Line(0, 63, 34, 63, Black);
			  ssd1306_Line(38, 63, 72, 63, Black);
			  break;
    		case 0x03:
			  ssd1306_Line(0, 38, 34, 38, Black);
			  ssd1306_Line(38, 38, 72, 38, Black);
			  ssd1306_Line(0, 63, 34, 63, White);
			  ssd1306_Line(38, 63, 72, 63, Black);
			  break;
    		case 0x04:
			  ssd1306_Line(0, 38, 34, 38, Black);
			  ssd1306_Line(38, 38, 72, 38, Black);
			  ssd1306_Line(0, 63, 34, 63, Black);
			  ssd1306_Line(38, 63, 72, 63, White);
			  break;
    		default:
			  ssd1306_Line(0, 38, 34, 38, Black);
			  ssd1306_Line(38, 38, 72, 38, Black);
			  ssd1306_Line(0, 63, 34, 63, Black);
			  ssd1306_Line(38, 63, 72, 63, Black);
			  break;
		}

    	/*ssd1306_SetCursor(0, 27);
	    ssd1306_WriteString(cSoil[0], Font_7x10, White);

  	    ssd1306_SetCursor(38, 27);
	    ssd1306_WriteString(cSoil[1], Font_7x10, White);

	    ssd1306_SetCursor(0, 52);
	    ssd1306_WriteString(cSoil[2], Font_7x10, White);

	    ssd1306_SetCursor(38, 52);
	    ssd1306_WriteString(cSoil[3], Font_7x10, White);*/

    	ssd1306_UpdateScreen();
    }

  }
  /* USER CODE END updateDisplayTask */
}

/* USER CODE BEGIN Header_readSensorsRTCTask */
/**
* @brief Function implementing the readSensorsRTC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_readSensorsRTCTask */
void readSensorsRTCTask(void *argument)
{
  /* USER CODE BEGIN readSensorsRTCTask */

  RTC_DateTypeDef rtc_date;
  RTC_TimeTypeDef rtc_time;

  float soilHumidity[4];

  // Initialize ADCs
  HAL_ADC_Start(&hadc1);
  /*HAL_ADC_Start(&hadc2);
  HAL_ADC_Start(&hadc3);
  HAL_ADC_Start(&hadc4);*/

  // Initialize DHT11
  init_dht11(&dht11, &htim7, DHT11_GPIO_Port, DHT11_Pin);

  // Initialize RTC
  rtc_date = (RTC_DateTypeDef){.WeekDay = 3, .Date = 12, .Month = 11, .Year = 24};
  rtc_time = (RTC_TimeTypeDef){.Hours = 23, .Minutes = 47, .Seconds= 0};

  HAL_RTC_SetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
  HAL_RTC_SetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);

	HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);

    //HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    /*HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
    HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
    HAL_ADC_PollForConversion(&hadc4, HAL_MAX_DELAY);*/

    soilHumidity[0] = (float) HAL_ADC_GetValue(&hadc1) * 100.0/4096.0;
    /*soilHumidity[1] = (float) HAL_ADC_GetValue(&hadc2) * 100.0/4096.0;
    soilHumidity[2] = (float) HAL_ADC_GetValue(&hadc3) * 100.0/4096.0;
    soilHumidity[3] = (float) HAL_ADC_GetValue(&hadc4) * 100.0/4096.0;*/

    readDHT11(&dht11);

    if(osMutexAcquire(mutexLogDataHandle, 100) == osOK)
    {
    	xLogData.s.year= rtc_date.Year;
    	xLogData.s.month = rtc_date.Month;
    	xLogData.s.day = rtc_date.Date;
    	xLogData.s.hour = rtc_time.Hours;
    	xLogData.s.minute = rtc_time.Minutes;
    	xLogData.s.second = rtc_time.Seconds;

    	xLogData.s.ambHumidity = dht11.humidty;
    	xLogData.s.ambTemperature = dht11.temperature;

    	xLogData.s.soilHumidity[0] = soilHumidity[0];
    	xLogData.s.soilHumidity[1] = soilHumidity[1];
    	xLogData.s.soilHumidity[2] = soilHumidity[2];
    	xLogData.s.soilHumidity[3] = soilHumidity[3];

    	osMutexRelease(mutexLogDataHandle);
    }
  }
  /* USER CODE END readSensorsRTCTask */
}

/* USER CODE BEGIN Header_controlWateringTask */
/**
* @brief Function implementing the controlWatering thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_controlWateringTask */
void controlWateringTask(void *argument)
{
  /* USER CODE BEGIN controlWateringTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(10);
    osThreadFlagsWait(0x0001, osFlagsWaitAll, osWaitForever);
	osMutexAcquire(mutexLogDataHandle, 500);
	if(xLogData.s.relaySelected > 0)
	{
		xLogData.s.relayStatus ^= (0x01 << (xLogData.s.relaySelected-1));
		HAL_GPIO_TogglePin(xRelayPorts[xLogData.s.relaySelected],
				           uiRelayPins[xLogData.s.relaySelected]);
	}
	osMutexRelease(mutexLogDataHandle);

  }
  /* USER CODE END controlWateringTask */
}

/* USER CODE BEGIN Header_updateLogTask */
/**
* @brief Function implementing the updateLog thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_updateLogTask */
void updateLogTask(void *argument)
{
  /* USER CODE BEGIN updateLogTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END updateLogTask */
}

/* USER CODE BEGIN Header_cfgWateringTask */
/**
* @brief Function implementing the cfgWatering thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_cfgWateringTask */
void cfgWateringTask(void *argument)
{
  /* USER CODE BEGIN cfgWateringTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END cfgWateringTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void ssd1306_WriteTemp(uint8_t temp, uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	char cTemp[3];

	sprintf(cTemp, "%d", temp);
	ssd1306_SetCursor(x, y);
	ssd1306_WriteString(cTemp, Font_11x18, color);
	ssd1306_SetCursor(x+2*11+1, y);
	ssd1306_WriteString("o", Font_7x10, color);
	ssd1306_SetCursor(x+3*11-3, y);
	ssd1306_WriteString("C", Font_11x18, color);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case BUTTON_SELECT_Pin:
			osMutexAcquire(mutexLogDataHandle, 500);
			if(xLogData.s.relaySelected++ == 4)
			{
				xLogData.s.relaySelected = 0;
			}
			osMutexRelease(mutexLogDataHandle);
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			break;

		case BUTTON_TOGGLE_Pin:
			osThreadFlagsSet(controlWateringHandle, 0x0001);
			break;
		default:
			break;
	}
}
/* USER CODE END Application */

