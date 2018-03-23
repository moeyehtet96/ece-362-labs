/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int btnPress = 0;
volatile int btnCurrent[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
volatile int btnPrevious[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int button;
int colNum = 1;
int sum = 0;
volatile int inputNum = 0;
int mappedBtn;
int tenPower = 0;
int buttonState[12];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void writeDigit(int number);
void matrixDebounce(void);
int btnDebounce(int butPos);
int mapButton(int btn);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void) {

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration----------------------------------------------------------*/

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
    MX_TIM1_Init();

    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        matrixDebounce();
        int x = 0;
        while (x <= 11) {
            if (buttonState[x] == 1) {
                buttonState[x] = 0;
                if (x == 9 || x == 11) {
                    if (x == 9) {
                        sum += inputNum;
                    } else if (x == 11) {
                        sum = 0;
                        inputNum = 0;
                    }
                    writeDigit(sum);
                    tenPower = 0;
                } else if (x != 9 && x != 11) {
                    mappedBtn = mapButton(x);
                    if (tenPower == 0) {
                        inputNum = mappedBtn;
                    } else {
                        for (int j = 1; j <= tenPower; j++) {
                            inputNum = (inputNum * 10) + mappedBtn;
                        }
                    }
                    writeDigit(inputNum);
                    tenPower++;
                }
            }
            x++;
        }
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* USER CODE END 3 */
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5|LD4_Pin|LD3_Pin
                          |GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6 
                           PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PF4 PF5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 LD4_Pin LD3_Pin 
                           PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|LD4_Pin|LD3_Pin
                          |GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10 
                           PB11 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void writeDigit(int number) {
    int ones, tens; // 1's and 10's variables of input number

    // Determine 1's and 10's
    ones = number % 10;
    tens = ((number - ones) / 10) % 10;

    // Reset the Pins
    HAL_GPIO_WritePin(GPIOB,
    GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
            GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA,
    GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
            GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_RESET);

    // Display 1's and 10's on respective 7-segment display
    if (tens == 0) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    } else if (tens == 1) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_10 | GPIO_PIN_2, GPIO_PIN_SET);
    } else if (tens == 2) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_1 | GPIO_PIN_0,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 3) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 4) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_10 | GPIO_PIN_2, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 5) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 6) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 7) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2, GPIO_PIN_SET);
    } else if (tens == 8) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (tens == 9) {
        HAL_GPIO_WritePin(GPIOB,
        GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    }

    if (ones == 0) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (ones == 1) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_6 | GPIO_PIN_5,
                GPIO_PIN_SET);
    } else if (ones == 2) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_SET);
    } else if (ones == 3) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
    } else if (ones == 4) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (ones == 5) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (ones == 6) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (ones == 7) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5,
                GPIO_PIN_SET);
    } else if (ones == 8) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5 | GPIO_PIN_4, GPIO_PIN_SET);
    } else if (ones == 9) {
        HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3,
                GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_SET);
    }
}

void matrixDebounce(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);

    btnCurrent[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    btnCurrent[3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    btnCurrent[6] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    btnCurrent[9] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);

    btnCurrent[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    btnCurrent[4] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    btnCurrent[7] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    btnCurrent[10] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);

    btnCurrent[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    btnCurrent[5] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    btnCurrent[8] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    btnCurrent[11] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
}

int btnDebounce(int butPos) {
    if ((btnPrevious[butPos] == 1) & (btnCurrent[butPos] == 0)) {
        btnPrevious[butPos] = btnCurrent[butPos];
        return 1;
    } else {
        btnPrevious[butPos] = btnCurrent[butPos];
        return 0;
    }
}

int mapButton(int btn) {
    int mappedVal;

    if(btn == 10) {
        mappedVal = 0;
    } else {
        mappedVal = btn + 1;
    }

    return mappedVal;
}

//extern void TIM1_BRK_UP_TRG_COM_IRQHandler() {
//    TIM1->SR &= ~(TIM_SR_UIF);
//
//    matrixDebounce(colNum);
//
//    if (colNum == 3) {
//        colNum = 1;
//    } else {
//        colNum++;
//    }
//
//    btnPress = btnDebounce(button);
//    btnPrevious = btnCurrent;
//}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
