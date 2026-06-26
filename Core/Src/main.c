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
#include "Altair_library_for_CubeIDE/altair.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* PAA5160E1 (OTOS) 定義 */
#define OTOS_I2C_ADDR         0x2E  /* (0x17 << 1) */
#define OTOS_REG_IMU_CALIB    0x06
#define OTOS_REG_RESET        0x07
#define OTOS_REG_POS_XL       0x20

/* BNO085 定義 */
#define BNO_I2C_ADDR          0x94  /* (0x4A << 1) */
#define BNO_REG_SET_FEATURE   0xFD
#define BNO_REPORT_ROTATION   0x05
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

USART_HandleTypeDef husart2;
USART_HandleTypeDef husart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART2_Init(void);
static void MX_USART3_Init(void);
/* USER CODE BEGIN PFP */
void USART2_Reinit_As_UART(void);
void UART2_SendString(const char* str);
void OTOS_Init(void);
void BNO085_Init(void);
void I2C3_Reset_And_Reinit(void);
void Read_Sensors(void);
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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART2_Init();
  MX_USART3_Init();
  /* USER CODE BEGIN 2 */
  USART2_Reinit_As_UART();
  OTOS_Init();
  BNO085_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Read_Sensors();
    HAL_Delay(20); /* 50Hz周期 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_4TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 3;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_4TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = ENABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = ENABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

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
  hi2c1.Init.ClockSpeed = 400000;
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  husart2.Instance = USART2;
  husart2.Init.BaudRate = 115200;
  husart2.Init.WordLength = USART_WORDLENGTH_8B;
  husart2.Init.StopBits = USART_STOPBITS_1;
  husart2.Init.Parity = USART_PARITY_NONE;
  husart2.Init.Mode = USART_MODE_TX_RX;
  husart2.Init.CLKPolarity = USART_POLARITY_LOW;
  husart2.Init.CLKPhase = USART_PHASE_1EDGE;
  husart2.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  if (HAL_USART_Init(&husart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  husart3.Instance = USART3;
  husart3.Init.BaudRate = 115200;
  husart3.Init.WordLength = USART_WORDLENGTH_8B;
  husart3.Init.StopBits = USART_STOPBITS_1;
  husart3.Init.Parity = USART_PARITY_NONE;
  husart3.Init.Mode = USART_MODE_TX_RX;
  husart3.Init.CLKPolarity = USART_POLARITY_LOW;
  husart3.Init.CLKPhase = USART_PHASE_1EDGE;
  husart3.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  if (HAL_USART_Init(&husart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USART2を非同期UART (115200bps, 8-N-1) として再初期化する関数 */
void USART2_Reinit_As_UART(void)
{
  /* USART2を無効化 */
  USART2->CR1 &= ~USART_CR1_UE;
  
  /* CR1設定: 8bit, No Parity, TX/RX有効, オーバーサンプリング16 */
  USART2->CR1 = USART_CR1_TE | USART_CR1_RE; 
  
  /* CR2設定: Stop bit 1, クロックピン無効 (CLKEN = 0) */
  USART2->CR2 = 0;
  
  /* CR3設定: ハードウェアフロー制御なし */
  USART2->CR3 = 0;
  
  /* ボーレート設定 (42MHz APB1, 115200bps -> 0x16D) */
  USART2->BRR = 0x16D; 
  
  /* USART2を有効化 */
  USART2->CR1 |= USART_CR1_UE;
}

/* UART2へ文字列を送信する関数 */
void UART2_SendString(const char* str)
{
  while (*str)
  {
    /* TXEビットがセットされるのを待つ */
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = *str++;
  }
}

/* PAA5160E1の初期化とキャリブレーション */
void OTOS_Init(void)
{
  uint8_t calib_val = 0xFF;
  uint8_t reset_val = 0x01;
  uint8_t progress = 0xFF;
  
  UART2_SendString("OTOS Calibration Starting...\r\n");
  
  /* IMUキャリブレーション要求 (255サンプル) */
  if (HAL_I2C_Mem_Write(&hi2c1, OTOS_I2C_ADDR, OTOS_REG_IMU_CALIB, I2C_MEMADD_SIZE_8BIT, &calib_val, 1, 100) != HAL_OK)
  {
    UART2_SendString("OTOS Connection Failed (Write Error).\r\n");
    return;
  }
  
  /* キャリブレーション完了待ち (0になるまで) - 最大100回ループ (約1秒) */
  int timeout = 100;
  do {
    HAL_Delay(10);
    if (HAL_I2C_Mem_Read(&hi2c1, OTOS_I2C_ADDR, OTOS_REG_IMU_CALIB, I2C_MEMADD_SIZE_8BIT, &progress, 1, 100) != HAL_OK)
    {
      UART2_SendString("OTOS Connection Failed (Read Error).\r\n");
      return;
    }
    timeout--;
  } while (progress != 0 && timeout > 0);
  
  if (timeout == 0)
  {
    UART2_SendString("OTOS Calibration Timeout.\r\n");
    return;
  }
  
  /* トラッキングリセット */
  HAL_I2C_Mem_Write(&hi2c1, OTOS_I2C_ADDR, OTOS_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_val, 1, 100);
  HAL_Delay(100); /* リセット後のセンサー内部処理安定待ち */
  
  UART2_SendString("OTOS Calibration Completed.\r\n");
}

/* BNO085の初期化 */
void BNO085_Init(void)
{
  uint8_t rx_buf[64];
  char scan_buf[64];
  
  UART2_SendString("BNO085 Initializing...\r\n");
  
  /* スキャン前に一度I2C3をリセット */
  I2C3_Reset_And_Reinit();
  
  /* I2C3バスのスキャン */
  UART2_SendString("Scanning I2C3 Bus...\r\n");
  for (uint8_t addr = 0x08; addr < 0x78; addr++)
  {
    HAL_StatusTypeDef res = HAL_I2C_IsDeviceReady(&hi2c3, (addr << 1), 3, 10);
    if (res == HAL_OK)
    {
      sprintf(scan_buf, "I2C3 Device found at 7-bit addr: 0x%02X (8-bit: 0x%02X)\r\n", addr, (addr << 1));
      UART2_SendString(scan_buf);
    }
    else if (res == HAL_BUSY)
    {
      UART2_SendString("I2C3 Scan Aborted: Bus is BUSY.\r\n");
      break;
    }
  }
  UART2_SendString("I2C3 Scan Finished.\r\n");
  
  /* 起動直後の不要なパケットを読み捨てる - 最大10回 */
  int discard_count = 0;
  while (HAL_I2C_Master_Receive(&hi2c3, BNO_I2C_ADDR, rx_buf, sizeof(rx_buf), 10) == HAL_OK && discard_count < 10)
  {
    HAL_Delay(5);
    discard_count++;
  }
  
  /* Game Rotation Vector (0x08) を50Hz (20000us) で有効にするコマンド */
  uint8_t bno_init_cmd[17] = {
    17, 0,             /* 長さ (17バイト) */
    2,                 /* チャンネル2 (Control) */
    0,                 /* シーケンス番号 */
    0xFD,              /* コマンド: Set Feature */
    0x08,              /* 機能: Game Rotation Vector */
    0,                 /* フラグ */
    0, 0,              /* 感度閾値 */
    0x20, 0x4E, 0, 0,  /* レポート間隔: 20000 us (0x4E20) */
    0, 0, 0, 0         /* バッチ間隔 (0) */
  };
  
  /* コマンドが成功するまでリトライ (最大10回, 各100msディレイ) */
  int init_retry = 10;
  HAL_StatusTypeDef init_status;
  do {
    init_status = HAL_I2C_Master_Transmit(&hi2c3, BNO_I2C_ADDR, bno_init_cmd, 17, 100);
    if (init_status != HAL_OK)
    {
      UART2_SendString("BNO085 Init Cmd Failed. Retrying...\r\n");
      HAL_Delay(100);
    }
    init_retry--;
  } while (init_status != HAL_OK && init_retry > 0);
  
  if (init_status == HAL_OK)
  {
    UART2_SendString("BNO085 Initialization Sent successfully.\r\n");
  }
  else
  {
    UART2_SendString("BNO085 Initialization Failed permanently.\r\n");
  }
}

/* I2C3ペリフェラルの強制リセットと再初期化 */
void I2C3_Reset_And_Reinit(void)
{
  char dbg_buf[64];
  
  /* 一時的にGPIOピンを通常入力/出力モードにして状態を確認する */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* まず入力(プルアップ)にして状態を読む */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  HAL_Delay(1);
  
  GPIO_PinState scl_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
  GPIO_PinState sda_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
  
  sprintf(dbg_buf, "GPIO Before check: SCL(PA8)=%d, SDA(PC9)=%d\r\n", scl_state, sda_state);
  UART2_SendString(dbg_buf);
  
  /* もしSDAがLOWなら、スレーブロックを解除するためにSCLを手動トグルする */
  if (sda_state == GPIO_PIN_RESET)
  {
    UART2_SendString("SDA is LOW. Attempting I2C bus recovery (clock toggling)...\r\n");
    
    /* SCL(PA8)をオープンドレイン出力(プルアップ)にする */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* SCLを9回トグルする */
    for (int i = 0; i < 9; i++)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
      HAL_Delay(1);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
      HAL_Delay(1);
    }
    
    /* 再度入力にしてSDAを確認 */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_Delay(1);
    
    scl_state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
    sda_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
    
    sprintf(dbg_buf, "GPIO After toggling: SCL(PA8)=%d, SDA(PC9)=%d\r\n", scl_state, sda_state);
    UART2_SendString(dbg_buf);
  }
  
  /* I2Cモードに再設定する（GPIO設定を上書きする） */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  UART2_SendString("Resetting I2C3...\r\n");
  
  /* I2C3の強制リセット */
  __HAL_RCC_I2C3_FORCE_RESET();
  HAL_Delay(5);
  __HAL_RCC_I2C3_RELEASE_RESET();
  HAL_Delay(5);
  
  /* 再初期化 */
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    UART2_SendString("I2C3 Re-init Failed.\r\n");
  }
  else
  {
    UART2_SendString("I2C3 Re-init Success.\r\n");
  }
}

/* センサー読み込みと送信 */
void Read_Sensors(void)
{
  char tx_buf[128];
  HAL_StatusTypeDef status;
  
  /* --- シリアル受信チェック (リセットコマンド) --- */
  static uint8_t otos_reset_skip_counter = 0;
  
  if (USART2->SR & USART_SR_RXNE)
  {
    uint8_t rx_data = USART2->DR;
    if (rx_data == 'R')
    {
      uint8_t reset_val = 0x01;
      HAL_I2C_Mem_Write(&hi2c1, OTOS_I2C_ADDR, OTOS_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_val, 1, 100);
      UART2_SendString("OTOS Reset Executed.\r\n");
      otos_reset_skip_counter = 5; /* リセット直後の5フレーム（約100ms）はデータ読み込みをスキップして0を送信する */
    }
  }
  
  /* --- PAA5160E1 (OTOS) 読み込み --- */
  if (otos_reset_skip_counter > 0)
  {
    /* リセット安定待ち期間中は0を送信 */
    sprintf(tx_buf, "O,0.0000,0.0000,0.0000\r\n");
    UART2_SendString(tx_buf);
    otos_reset_skip_counter--;
  }
  else
  {
    uint8_t otos_data[6] = {0};
    status = HAL_I2C_Mem_Read(&hi2c1, OTOS_I2C_ADDR, OTOS_REG_POS_XL, I2C_MEMADD_SIZE_8BIT, otos_data, 6, 10);
    if (status == HAL_OK)
    {
      int16_t x_raw = (int16_t)(otos_data[0] | (otos_data[1] << 8));
      int16_t y_raw = (int16_t)(otos_data[2] | (otos_data[3] << 8));
      int16_t h_raw = (int16_t)(otos_data[4] | (otos_data[5] << 8));
      
      float x = x_raw * (10.0f / 32768.0f);
      float y = y_raw * (10.0f / 32768.0f);
      float h = h_raw * (3.14159265f / 32768.0f);
      
      sprintf(tx_buf, "O,%.4f,%.4f,%.4f\r\n", x, y, h);
      UART2_SendString(tx_buf);
    }
    else if (status == HAL_BUSY)
    {
      /* I2C1がBUSYで固まった場合は再初期化して復旧 */
      HAL_I2C_Init(&hi2c1);
    }
  }
  
  /* センサー間の十分なディレイ（I2Cバスの安定化） */
  HAL_Delay(10);
  
  /* --- BNO085 読み込み --- */
  uint8_t bno_data[64] = {0};
  status = HAL_I2C_Master_Receive(&hi2c3, BNO_I2C_ADDR, bno_data, 64, 50);
  if (status == HAL_OK)
  {
    /* チャンネル3 (Sensor Reports) */
    if (bno_data[2] == 3)
    {
      /* SHTPパケット内のReport ID (0x08) の位置をスキャンする
         バッチ処理によりタイムベースレポート(0xFB)などが先頭に混ざることがあるため、
         バッファの4〜19バイト目を探索して 0x08 (Game Rotation Vector ID) を探します。 */
      int rv_idx = -1;
      for (int i = 4; i < 20; i++)
      {
        if (bno_data[i] == 0x08)
        {
          rv_idx = i;
          break;
        }
      }
      
      /* ID 0x08 が見つかり、かつ残りのクォータニオンデータ(8バイト)がバッファ内にある場合 */
      if (rv_idx != -1 && (rv_idx + 10) < 64)
      {
        int16_t i_raw = (int16_t)(bno_data[rv_idx + 3] | (bno_data[rv_idx + 4] << 8));
        int16_t j_raw = (int16_t)(bno_data[rv_idx + 5] | (bno_data[rv_idx + 6] << 8));
        int16_t k_raw = (int16_t)(bno_data[rv_idx + 7] | (bno_data[rv_idx + 8] << 8));
        int16_t r_raw = (int16_t)(bno_data[rv_idx + 9] | (bno_data[rv_idx + 10] << 8));
        
        float quat_i = i_raw / 16384.0f;
        float quat_j = j_raw / 16384.0f;
        float quat_k = k_raw / 16384.0f;
        float quat_r = r_raw / 16384.0f;
        
        sprintf(tx_buf, "B,%.4f,%.4f,%.4f,%.4f\r\n", quat_i, quat_j, quat_k, quat_r);
        UART2_SendString(tx_buf);
      }
    }
  }
  else
  {
    /* HAL_OK以外の場合、BUSYのときのみ再初期化を行う */
    uint32_t err_code = HAL_I2C_GetError(&hi2c3);
    if (status == HAL_BUSY)
    {
      sprintf(tx_buf, "BNO_ERR: status=%d, err=%lu\r\n", status, err_code);
      UART2_SendString(tx_buf);
      I2C3_Reset_And_Reinit();
    }
  }
  
  /* 通信後の十分なディレイ */
  HAL_Delay(10);
}
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
