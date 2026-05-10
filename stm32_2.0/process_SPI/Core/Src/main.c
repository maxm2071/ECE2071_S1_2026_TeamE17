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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define MAX_LEN 1000
#define MOVING_LEN 10
#define OUTLIER 2048


uint8_t buffer[2][MAX_LEN];
int SPI_rec = 0;
int UART_trans;
int buffer_index = 0;
uint16_t received_byte[2];
int is_UARTING = 0;
int buffer_flag = 0;
//uint32_t average_sum = 0;


int is_distanceTrigger = 0;
int is_sendValid = 1;
uint16_t average_array[MOVING_LEN] = {0};
uint8_t comp_buffer;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
uint8_t get_average(uint16_t array[], int len, uint16_t value){
	 for(int i = 1; i < len; i++){
	        array[i-1] = array[i];
	    }
	 array[len-1] = value;
    uint32_t sum = 0;
    for (int i = 0; i < len; i++){
        sum += array[i];
    }
    uint16_t average = sum/len;
    return (uint8_t)average;
}

void change_array(uint16_t array[], int len, uint16_t value){
    for(int i = 1; i < len; i++){
        array[i-1] = array[i];
    }
    array[len-1] = value;
}
*/
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){

	if (buffer_index < MAX_LEN){
		//get average in line due to it being faster than calling functions as no jumping is required
		uint16_t down_sample = (received_byte[0] + received_byte[1])/2;
		if(abs((int)down_sample-OUTLIER) > 2000){
			buffer[SPI_rec][buffer_index] = (uint8_t)(128);



		}else{
		uint32_t average_sum = down_sample;
		for (int i = 0; i<MOVING_LEN-1; i++){
			average_sum += average_array[i];
			average_array[i]= average_array[i+1];
		}
		average_array[MOVING_LEN-1] = down_sample;



		//1 array 2 buffers, SPI_rec switches between 1 and 0 depending on which one is being
		//UARTed (got idea of using a 2 index array instead of 2 separate arrays but implementation is my own)
		buffer[SPI_rec][buffer_index] = (uint8_t)((average_sum/MOVING_LEN)>>4);
		}

		buffer_index++;
	}
	//buffer is full check that UARTing is complete
	if(buffer_index >= MAX_LEN && is_sendValid == 1 && is_UARTING == 0){
		//raise buffer flag so CPU knows it can start UARTing again
		buffer_flag = 1;
		//switch array to receive into
		SPI_rec = SPI_rec ^1;
		//buffer index goes back to 0
		buffer_index = 0;


		}
	//call spi receive again so that interrupt will fire again
	HAL_SPI_Receive_IT(&hspi1, received_byte, 2);

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	//let SPI callback know that it can switch the arrays
	is_UARTING = 0;

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	//receiving this means the user is deciding between distance trigger mode and regular
	HAL_UART_Receive_IT(&huart2, &comp_buffer, 1);
	//buffer_index = 0;
	//distance trigger logic
	if (comp_buffer == '1'){

		is_distanceTrigger = 1;
	}else{
		is_distanceTrigger = 0;
		is_sendValid = 1;
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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //start timers, start interrupt functions initialise US sensor variables
	HAL_UART_Receive_IT(&huart2, &comp_buffer, 1);
	HAL_SPI_Receive_IT(&hspi1, received_byte, 2);
	HAL_TIM_Base_Start(&htim16);
	__HAL_TIM_SET_COUNTER(&htim16, 0);
	float time;
	float distance;
  while (1)
  {
	  //buffer_flag means UARTing should commence
	  if (buffer_flag == 1){
		  //XOR to flip bit from 1 to 0 or back to know which array to send
		  //opposite of the array that is currently being filled
		  UART_trans = SPI_rec ^ 1;
		  //set global variables so that array being sent isnt being received
		  //to and make sure system doesnt try send an array again
		  is_UARTING = 1;
		  buffer_flag = 0;
		  HAL_UART_Transmit_IT(&huart2, buffer[UART_trans], MAX_LEN);
	  }
		if (is_distanceTrigger == 1){
			//HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
		  if (__HAL_TIM_GET_COUNTER(&htim16) <= 10){
			  HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, 1);
		  }else{
			  HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, 0);
			  //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
			  if (HAL_GPIO_ReadPin(echo_GPIO_Port, echo_Pin) == 1){
				  __HAL_TIM_SET_COUNTER(&htim16, 0);
				  //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
				  while(HAL_GPIO_ReadPin(echo_GPIO_Port, echo_Pin) == 1){
					  continue;
				  }
				  time = __HAL_TIM_GET_COUNTER(&htim16);
				  distance = (time/58.309);
				  //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
				  if (distance < 10){
					  	  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
					  	  is_sendValid = 1;
					  } else {
						  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
						  is_sendValid = 0;
					  }
				  __HAL_TIM_SET_COUNTER(&htim16, 0);
				  while (__HAL_TIM_GET_COUNTER(&htim16) < 60000) {
					  if (buffer_flag == 1){
						  //XOR to flip bit from 1 to 0 or back to know which array to use
						  UART_trans = SPI_rec ^ 1;
						  is_UARTING = 1;
						  HAL_UART_Transmit_IT(&huart2, buffer[UART_trans], MAX_LEN);
						  buffer_flag = 0;
					  }
				  }

				  __HAL_TIM_SET_COUNTER(&htim16, 0);
				  }

			  }
		  }

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_12BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 31;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  huart2.Init.BaudRate = 921600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(trigger_GPIO_Port, trigger_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : echo_Pin */
  GPIO_InitStruct.Pin = echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(echo_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : trigger_Pin */
  GPIO_InitStruct.Pin = trigger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(trigger_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
