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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
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
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_uart5_rx;

/* USER CODE BEGIN PV */
bool tx_key_flag=1;
bool tx_arm_flag=1;
bool tx_launch_flag=1;
bool tx_ignition_flag=1;
bool tx_error_flag=1;
bool tx_init_flag=1;
bool tx_kill_flag=1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART5_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char tx_data[2];
char rx_data[2];
char rx_arduino[2];
bool Sec_key = 0, Arm_switch = 0, Ign_switch = 0;
bool seq_flag = 0;
bool ignition = 0;
//ACK Sequence
//char key[2]='FF';
//char arm[2]='BB';
//char isu_check[2]='DD';
//char kill[2]='KK';
//char ignition_switch[2]='II;
//char launch[2]='LL';
void seq_check()
	{
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == 0 && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)== 0 && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 0)
			{seq_flag = 1;
			tx_key_flag=1;
			tx_arm_flag=1;
			tx_launch_flag=1;
			tx_ignition_flag=1;
			tx_init_flag=1;
			tx_error_flag=1;
			tx_kill_flag=1;
			}
		else if(Sec_key==0 && HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == 1) seq_flag=0;
	}
void error()
{
	memset(tx_data,'F',2);
	HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
	if(tx_error_flag==1){
	HAL_Delay(3000);
	HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
	memset(rx_data,'\0',2);
	tx_error_flag=0;
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
  MX_DMA_Init();
  MX_UART5_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
HAL_UART_Receive_DMA(&huart5, rx_data, sizeof(rx_data));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		seq_check();
		if(seq_flag ==1){
			if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == 0 || Sec_key == 0)
			{
			memset(tx_data,'N',2); //Initialisation
			    HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
			    if(tx_init_flag==1){
				HAL_Delay(3000);
				HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
				memset(rx_data,'\0',2);
				tx_init_flag=0;
			}
			}
			if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == 1 || Sec_key == 1)
			{
				Sec_key=1;
				if(Arm_switch==0){
				memset(tx_data,'K',2); //Security Key ACK
				HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
				if(tx_key_flag==1){
				HAL_Delay(3000);
				HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
				memset(rx_data,'\0',2);
				tx_key_flag=0;
				}
				}
			}
				if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == 0 && Arm_switch == 0){
					Sec_key = 0; // Notify the user to reset the inputs
					seq_flag = 0;
					Arm_switch = 0;
					Ign_switch = 0;
				}
				if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10)== 1 || Arm_switch == 1){
						Arm_switch = 1;
						if(Ign_switch==0){
						memset(tx_data,'A',2); //Arm Switch ACK
						HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
						if(tx_arm_flag==1){
						HAL_Delay(3000);
						HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
						memset(rx_data,'\0',2);
						tx_arm_flag=0;
						}
						}
				}
						if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == 0){
							Arm_switch = 0;
						}
						if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 1 || Ign_switch == 1 ){
//							HAL_Delay(5000);
							if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == 1) {
								memset(tx_data,'\0',2);
								Ign_switch = 1;
								memset(tx_data,'I',2);

								HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
								if(tx_ignition_flag==1){
								HAL_Delay(3000);
								HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
								memset(rx_data,'\0',2);
								tx_ignition_flag=0;
//								HAL_Delay(100);
								}
								if(rx_data[0]=='L')
								{
								memset(tx_data,'L',2); //Launching ACK
								HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
								if(tx_launch_flag==1){
								HAL_Delay(3000);
								HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
								memset(rx_data,'\0',2);
								tx_launch_flag=0;
//								HAL_Delay(100);
								}
								}
								memset(tx_data,'\0',2);
								HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11,1);
								HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,0);
							HAL_Delay(2000);
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);
							HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11,0);
							}
							}
		}
							else
							{
								Sec_key = 0;
								Arm_switch = 0;
								Ign_switch = 0;
								seq_flag = 0;
								error();
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
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(kill_led_GPIO_Port, kill_led_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Security_key_Pin PC8 Arm_Switch_Pin */
  GPIO_InitStruct.Pin = Security_key_Pin|GPIO_PIN_8|Arm_Switch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : kill_led_Pin */
  GPIO_InitStruct.Pin = kill_led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(kill_led_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Buzzer_Pin */
  GPIO_InitStruct.Pin = Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Buzzer_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)==1)
  {
        memset(tx_data,'Y',2); //Kill_Switch
    	HAL_UART_Transmit(&huart5, tx_data, strlen(tx_data),100);
    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
    	if(tx_error_flag==1){
    	HAL_Delay(3000);
    	HAL_UART_Transmit(&huart4, rx_data, strlen(rx_data),100);
    	memset(rx_data,'\0',2);
    	tx_kill_flag=0;
    	}
  }
  HAL_Delay(500);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
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
