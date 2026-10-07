
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
  * COPYRIGHT(c) 2026 STMicroelectronics
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
#include "stm32f4xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define M_DO  262
#define M_RE  294
#define M_MI  330
#define M_FA  349
#define M_SOL 392
#define M_LA  440
#define M_SI  494
#define M_DO_H 523
#define PAUSE 0

#define BEAT_1 400
#define BEAT_2 200
#define BEAT_4 100

int music_score[][2] = {
  {M_SI, BEAT_2}, {M_DO_H, BEAT_2}, {M_RE, BEAT_1},
  {M_SI, BEAT_2}, {M_DO_H, BEAT_2}, {M_RE, BEAT_1},
  {M_SI, BEAT_2}, {M_DO_H, BEAT_2}, {M_RE, BEAT_2}, {M_DO_H, BEAT_2}, {M_SI, BEAT_1},
  {M_LA, BEAT_1}, {M_SOL, BEAT_1},
  {M_SOL, BEAT_2}, {M_LA, BEAT_2}, {M_SI, BEAT_1},
  {PAUSE, BEAT_2}
};
uint8_t is_playing = 1; // 1=正在播放音乐, 0=音乐休息
uint32_t music_timer = 0; // 音乐计时
int music_index = 0;      // 当前播到第几个音符
uint32_t music_rest_timer = 0; // 音乐休息计时

uint32_t led_timer = 0;   // LED计时
uint8_t led_state = 0;    // 0=亮, 1=灭
int8_t led_index = 0;     // 当前灯索引
uint32_t led_delay_time = 100; // 灯变化间隔

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
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
  MX_DMA_Init();
  MX_USART6_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM12_Init();
  /* USER CODE BEGIN 2 */
  //HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

    GPIO_TypeDef* LED_Port[8] = {GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG};
    uint16_t LED_Pin[8] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,GPIO_PIN_8};

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

    /* USER CODE BEGIN WHILE */
    uint32_t current_tick = HAL_GetTick(); // 获取当前时间

    /* ==================== 任务 1：播放音乐 ==================== */
    if (is_playing == 1) // 如果正在播放
    {
        // 检查当前音符是否唱完
        if (current_tick - music_timer >= music_score[music_index][1])
        {
            music_timer = current_tick; // 重置音乐计时
            music_index++;              // 下一个音符

            if (music_index < sizeof(music_score) / sizeof(music_score[0]))
            {
                // 播放下一个音符
                uint16_t freq = music_score[music_index][0];
                if (freq == 0) {
                    HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1); // 休止符
                } else {
                    uint32_t arr = 1000000 / freq - 1;
                    __HAL_TIM_SET_AUTORELOAD(&htim12, arr);
                    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, arr / 2);
                    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
                }
            }
            else
            {
                // 整首歌播完，进入休息状态
                HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
                is_playing = 0; // 标记为休息
                music_rest_timer = current_tick; // 记录休息开始时间
            }
        }
    }
    else // 如果正在休息
    {
        // 检查休息是否满2秒
        if (current_tick - music_rest_timer >= 500)
        {
            is_playing = 1;    // 重新开始播放
            music_index = 0;   // 乐谱从头开始
            music_timer = current_tick; // 重置计时
        }
    }

    /* ==================== 任务 2：LED 跑马灯 ==================== */
    // 如果时间到了，就改变灯的状态
    if (current_tick - led_timer >= led_delay_time)
    {
        led_timer = current_tick; // 重置LED计时器

        if (led_state == 0) // 点亮阶段
        {
            HAL_GPIO_WritePin(LED_Port[led_index], LED_Pin[led_index], GPIO_PIN_RESET); // 点亮当前灯
            led_index++;

            if (led_index > 7) { // 8个灯都点亮了
                led_state = 1;           // 切换到熄灭阶段
                led_index = 0;           // 重置索引，从第0个开始灭
                led_delay_time = 200;   // 全亮后先等1秒再开始灭
            }
        }
        else // 熄灭阶段
        {
            // 先熄灭当前灯
            HAL_GPIO_WritePin(LED_Port[led_index], LED_Pin[led_index], GPIO_PIN_SET);
            led_index++;

            if (led_index > 7) { // 8个灯都熄灭了
                led_state = 0;           // 回到点亮阶段
                led_index = 0;           // 重置索引，从第0个开始亮
                led_delay_time = 200;   // 全灭后先等1秒再开始亮
            } else {
                // 如果还在熄灭过程中，保持正常的500ms节奏
                led_delay_time = 200;
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
