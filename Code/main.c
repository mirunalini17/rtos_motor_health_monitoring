/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"
#include "mpu6050.h"
#include <math.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define TEMP_WARNING_THRESHOLD   40.0f
#define TEMP_CRITICAL_THRESHOLD  50.0f

#define VIB_WARNING_THRESHOLD   0.05f
#define VIB_CRITICAL_THRESHOLD  0.15f
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
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart2;



/* Definitions for MonitoringTASK */
osThreadId_t MonitoringTASKHandle;
const osThreadAttr_t MonitoringTASK_attributes = {
  .name = "MonitoringTASK",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for TemperatureTASK */
osThreadId_t TemperatureTASKHandle;
const osThreadAttr_t TemperatureTASK_attributes = {
  .name = "TemperatureTASK",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for VibrationTASK */
osThreadId_t VibrationTASKHandle;
const osThreadAttr_t VibrationTASK_attributes = {
  .name = "VibrationTASK",
  .stack_size = 2048,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* USER CODE BEGIN PV */
osMessageQueueId_t TemperatureQueueHandle;

float received_temperature = 0.0f;

osMessageQueueId_t VibrationQueueHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
void MonitoringTask(void *argument);
void TemperatureTask(void *argument);
void VibrationTask(void *argument);

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  if (MPU6050_Init(&hi2c1) != HAL_OK)
  {
      Error_Handler();
  }
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE BEGIN RTOS_QUEUES */

  TemperatureQueueHandle = osMessageQueueNew(5, sizeof(float), NULL);

  VibrationQueueHandle = osMessageQueueNew(5, sizeof(float), NULL);

  if (TemperatureQueueHandle == NULL || VibrationQueueHandle == NULL)
  {
      Error_Handler();
  }

  /* USER CODE END RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MonitoringTASK */
  MonitoringTASKHandle = osThreadNew(MonitoringTask, NULL, &MonitoringTASK_attributes);

  /* creation of TemperatureTASK */
  TemperatureTASKHandle = osThreadNew(TemperatureTask, NULL, &TemperatureTASK_attributes);

  /* creation of VibrationTASK */
  VibrationTASKHandle = osThreadNew(VibrationTask, NULL, &VibrationTASK_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 1249;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_MonitoringTask */
/**
  * @brief  Function implementing the MonitoringTASK thread.
  * @param  argument: Not used
  * @retval None
  */
float Calculate_RMS(float *samples, uint16_t count)
{
    float sum = 0.0f;

    for (uint16_t i = 0; i < count; i++)
    {
        sum += samples[i] * samples[i];
    }

    return sqrtf(sum / count);
}
/* USER CODE END Header_MonitoringTask */

void MonitoringTask(void *argument)
{
	uint8_t temp_status = 0;
	uint8_t vibration_status = 0;

	uint8_t temp_received = 0;
	uint8_t vibration_received = 0;
	uint8_t health_update = 0;
    float temperature;
    MPU6050_Data vibration;
    float vibration_rms;
    char msg[160];

    for (;;)
    {
        // Receive temperature
        if (osMessageQueueGet(TemperatureQueueHandle,
                              &temperature,
                              NULL,
                              0U) == osOK)
        {
        	temp_received = 1;
        	health_update = 1;

        	if (temperature >= TEMP_CRITICAL_THRESHOLD)
        	{
        	    temp_status = 2;
        	}
        	else if (temperature >= TEMP_WARNING_THRESHOLD)
        	{
        	    temp_status = 1;
        	}
        	else
        	{
        	    temp_status = 0;
        	}
        	if (temperature >= TEMP_CRITICAL_THRESHOLD)
        	{
        	    snprintf(msg, sizeof(msg),
        	             "CRITICAL FAULT: Motor Overtemperature! %.2f C\r\n",
        	             temperature);
        	}
        	else if (temperature >= TEMP_WARNING_THRESHOLD)
        	{
        	    snprintf(msg, sizeof(msg),
        	             "WARNING: Motor Temperature High! %.2f C\r\n",
        	             temperature);
        	}
        	else
        	{
        	    snprintf(msg, sizeof(msg),
        	             "Motor Temperature: %.2f C | NORMAL\r\n",
        	             temperature);
        	}

        	HAL_UART_Transmit(&huart2,
        	                  (uint8_t *)msg,
        	                  strlen(msg),
        	                  HAL_MAX_DELAY);

        }

        // Receive vibration data


        if (osMessageQueueGet(VibrationQueueHandle,
                              &vibration_rms,
                              NULL,
                              0U) == osOK)
        {
        	vibration_received = 1;
        	health_update = 1;

        	if (vibration_rms >= VIB_CRITICAL_THRESHOLD)
        	{
        	    vibration_status = 2;
        	}
        	else if (vibration_rms >= VIB_WARNING_THRESHOLD)
        	{
        	    vibration_status = 1;
        	}
        	else
        	{
        	    vibration_status = 0;
        	}
        	if (vibration_rms >= VIB_CRITICAL_THRESHOLD)
        	{
        	    snprintf(msg, sizeof(msg),
        	             "CRITICAL FAULT: Excessive Vibration! %.6f g\r\n",
        	             vibration_rms);
        	}
        	else if (vibration_rms >= VIB_WARNING_THRESHOLD)
        	{
        	    snprintf(msg, sizeof(msg),
        	             "WARNING: High Vibration! %.6f g\r\n",
        	             vibration_rms);
        	}
        	else
        	{
        	    snprintf(msg, sizeof(msg),
        	             "Vibration RMS: %.6f g | NORMAL\r\n",
        	             vibration_rms);
        	}

        	HAL_UART_Transmit(&huart2,
        	                  (uint8_t *)msg,
        	                  strlen(msg),
        	                  HAL_MAX_DELAY);
        }
        if (health_update && temp_received && vibration_received)
        {
            health_update = 0;

            if (temp_status == 2 && vibration_status == 2)
            {
                snprintf(msg, sizeof(msg),
                         "\r\nMOTOR HEALTH: CRITICAL - Temperature and Vibration!\r\n");
            }
            else if (temp_status == 2)
            {
                snprintf(msg, sizeof(msg),
                         "\r\nMOTOR HEALTH: CRITICAL - High Temperature!\r\n");
            }
            else if (vibration_status == 2)
            {
                snprintf(msg, sizeof(msg),
                         "\r\nMOTOR HEALTH: CRITICAL - Excessive Vibration!\r\n");
            }
            else if (temp_status == 1 || vibration_status == 1)
            {
                snprintf(msg, sizeof(msg),
                         "\r\nMOTOR HEALTH: WARNING - Abnormal Condition!\r\n");
            }
            else
            {
                snprintf(msg, sizeof(msg),
                         "\r\nMOTOR HEALTH: NORMAL\r\n");
            }

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)msg,
                              strlen(msg),
                              HAL_MAX_DELAY);
        }
        osDelay(100);
    }
}


/* USER CODE BEGIN Header_TemperatureTask */
/**
* @brief Function implementing the TemperatureTASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TemperatureTask */

void TemperatureTask(void *argument)
{
    uint32_t adc_value;
    float temperature;

    HAL_ADCEx_Calibration_Start(&hadc1);

    for (;;)
    {
        HAL_ADC_Start(&hadc1);

        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
        {
            adc_value = HAL_ADC_GetValue(&hadc1);

            temperature = (adc_value * 3.3f * 100.0f)
                          / 4095.0f;

            osMessageQueuePut(
                TemperatureQueueHandle,
                &temperature,
                0U,
                0U
            );

        }


        HAL_ADC_Stop(&hadc1);

        osDelay(1000);
    }
}


/* USER CODE BEGIN Header_VibrationTask */
/**
* @brief Function implementing the VibrationTASK thread.
* @param argument: Not used
* @retval None
*/

/* USER CODE END Header_VibrationTask */
void VibrationTask(void *argument)
{
    MPU6050_Data vibration;

    float ax_samples[100];
    float ay_samples[100];
    float az_samples[100];

    float mean_ax, mean_ay, mean_az;
    float sum, vibration_rms;

    uint16_t i;
    uint16_t valid_samples;

    for (;;)
    {
        mean_ax = 0.0f;
        mean_ay = 0.0f;
        mean_az = 0.0f;
        valid_samples = 0;

        /* Collect 100 valid samples */
        for (i = 0; i < 100; i++)
        {
            if (MPU6050_Read(&hi2c1, &vibration) == HAL_OK)
            {
                ax_samples[valid_samples] = vibration.ax;
                ay_samples[valid_samples] = vibration.ay;
                az_samples[valid_samples] = vibration.az;

                mean_ax += vibration.ax;
                mean_ay += vibration.ay;
                mean_az += vibration.az;

                valid_samples++;
            }

            osDelay(10);
        }

        /* Calculate RMS only if samples were received */
        if (valid_samples > 0)
        {
            mean_ax /= valid_samples;
            mean_ay /= valid_samples;
            mean_az /= valid_samples;

            sum = 0.0f;

            for (i = 0; i < valid_samples; i++)
            {
                float dx = ax_samples[i] - mean_ax;
                float dy = ay_samples[i] - mean_ay;
                float dz = az_samples[i] - mean_az;

                sum += dx * dx + dy * dy + dz * dz;
            }

            vibration_rms = sqrtf(sum / valid_samples);

            osMessageQueuePut(
                VibrationQueueHandle,
                &vibration_rms,
                0U,
                0U
            );
        }

        osDelay(100);
    }
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
