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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h>

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

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

const uint8_t mini_font[15][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Index 0: ' '
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // Index 1: 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // Index 2: 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // Index 3: 'C'
	{0x7F, 0x41, 0x41, 0x22, 0x1C}, // Index 4: 'D'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // Index 4: '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // Index 5: '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // Index 6: '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // Index 7: '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // Index 8: '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // Index 9: '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // Index 10: '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // Index 11: '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // Index 12: '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}  // Index 13: '9'
};

void OLED_DrawChar(char character, uint8_t x, uint8_t page)
{
	uint8_t index = 0;
	if (character == ' ') index = 0;
	else if (character >= 'A' && character <= 'D') index = (character - 'A') + 1;
	else if (character >= '0' && character <= '9') index = (character - '0') + 5;
	else return;

	uint8_t set_window_cmds[] = {
		0x21, x, x + 4,
		0x22, page, page
	};
	HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x00, 1, set_window_cmds, sizeof(set_window_cmds), 100);

	uint8_t char_data[5];
	for (int i = 0; i < 5; i++) {
		char_data[i] = mini_font[index][i];
	}

	HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x40, 1, char_data, sizeof(char_data), 100);
}

void OLED_DrawCursor(uint8_t x, uint8_t page, uint8_t color)
{
	if (x > 120) x = 120;
	if (page > 7) page = 7;

	uint8_t set_window_cmds[] = {
		0x21, // Set Column Address
		x,   // X-Start
		x + 7,   // X-End
		0x22, // Set Page Address
		page,    // Y-Start
		page     // Y-End
	};

	HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x00, 1, set_window_cmds, sizeof(set_window_cmds), 100);
	uint8_t block_data[8];
	memset(block_data, color, sizeof(block_data));
	HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x40, 1, block_data, sizeof(block_data), 100);
}

void OLED_Print(char* str, uint8_t x, uint8_t page) {
    int i = 0;

    while (str[i] != '\0') {
        OLED_DrawChar(str[i], x, page);
        x += 6;
        i++;

        if (x > 122) break;
    }
}

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
  /* USER CODE BEGIN 2 */

  uint8_t init_cmds[] = {
        0xAE, // Display OFF
        0x20, 0x00, // Memory Addressing Mode: Horizontal
        0x81, 0xFF, // max contrast
        0xA1, // Segment Remap (mirror x axis)
        0xC8, // COM Output Scan Direction (mirror y axis)
        0x8D, 0x14, // Charge Pump
        0xAF  // Display ON
    };

  HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x00, 1, init_cmds, sizeof(init_cmds), 100);

  uint8_t reset_window[] = {
	0x21, 0, 127,
	0x22, 0, 7
  };
  HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x00, 1, reset_window, sizeof(reset_window), 100);
  uint8_t zero_buffer[1024];
  memset(zero_buffer, 0, sizeof(zero_buffer));
  HAL_I2C_Mem_Write(&hi2c1, (0x3C << 1), 0x40, 1, zero_buffer, sizeof(zero_buffer), 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  ADC_ChannelConfTypeDef sConfig = {0};

  while (1)
  {
	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  uint32_t adc_x = HAL_ADC_GetValue(&hadc1);

	  sConfig.Channel = ADC_CHANNEL_4;
	  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  uint32_t adc_y = HAL_ADC_GetValue(&hadc1);

	  uint8_t btn_val = HAL_GPIO_ReadPin(JOYSTICK_BTN_GPIO_Port, JOYSTICK_BTN_Pin);

	  char str_x[20];
	  char str_y[20];
	  char str_btn[20];

	  sprintf(str_x, "X: %lu    ", adc_x);
	  sprintf(str_y, "Y: %lu    ", adc_y);
	  sprintf(str_btn, "BTN: %d  ", btn_val);

	  OLED_Print(str_x, 10, 1);
	  OLED_Print(str_y, 10, 3);
	  OLED_Print(str_btn, 10, 5);

	  HAL_Delay(50);

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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
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

  /*Configure GPIO pin : JOYSTICK_BTN_Pin */
  GPIO_InitStruct.Pin = JOYSTICK_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(JOYSTICK_BTN_GPIO_Port, &GPIO_InitStruct);

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
