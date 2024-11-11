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
/* Definitions for configureWateri */
osThreadId_t configureWateriHandle;
const osThreadAttr_t configureWateri_attributes = {
  .name = "configureWateri",
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

/* USER CODE END FunctionPrototypes */

void transmitUARTTask(void *argument);
void updateDisplayTask(void *argument);
void readSensorsRTCTask(void *argument);
void controlWateringTask(void *argument);
void updateLogTask(void *argument);
void configureWateringTask(void *argument);

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

  /* creation of configureWateri */
  configureWateriHandle = osThreadNew(configureWateringTask, NULL, &configureWateri_attributes);

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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
    osDelay(1);
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

/* USER CODE BEGIN Header_configureWateringTask */
/**
* @brief Function implementing the configureWateri thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_configureWateringTask */
void configureWateringTask(void *argument)
{
  /* USER CODE BEGIN configureWateringTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END configureWateringTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

