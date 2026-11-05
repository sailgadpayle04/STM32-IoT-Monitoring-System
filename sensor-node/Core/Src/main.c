/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Day 1 sensor node firmware.
  *                   Initializes MCU, clocks, GPIO, USART1, USART2, and I2C1.
  *                   Verifies serial communication and echo functionality.
  * @project        : Two-Node STM32 Monitoring System
  * @author         : Embedded Firmware Team
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 Embedded Firmware Team.
  * All rights reserved.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart_debug.h"

/* Private Variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
I2C_HandleTypeDef  hi2c1;

/* Static 1-byte buffer for debug UART interrupt reception */
static uint8_t rx_byte = 0;

/* Private Function Prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_I2C1_Init(void);

/* Application Entry Point ---------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock: HSE 25MHz -> PLL -> 100MHz SYSCLK */
    SystemClock_Config();

    /* Initialize configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();

    /* Initialize Debug UART driver with USART2 handle */
    Debug_Init(&huart2);

    /* Transmit Day 1 startup banner */
    Debug_SendString("\r\n");
    Debug_SendString("========================================\r\n");
    Debug_SendString("  Two-Node STM32 Monitoring System\r\n");
    Debug_SendString("  Sensor Node - STM32F411\r\n");
    Debug_SendString("  Firmware v1.0 - Day 1\r\n");
    Debug_SendString("========================================\r\n");

    /* Report actual clock frequency from HAL — verifies PLL config is correct */
    Debug_SendFormat("[INIT] System clock: %lu MHz\r\n",
                     (unsigned long)(HAL_RCC_GetSysClockFreq() / 1000000U));
    Debug_SendString("[INIT] UART2 debug: 115200 baud (PA2 TX / PA3 RX)\r\n");
    Debug_SendString("[INIT] UART1 inter-node: 115200 baud (PA9 TX / PA10 RX)\r\n");
    Debug_SendString("[INIT] I2C1 sensor bus: 400 kHz (PB6 SCL / PB7 SDA)\r\n");
    Debug_SendString("[INIT] All peripherals initialized\r\n");
    Debug_SendString("[INIT] LED test: 3 blinks\r\n");

    /* Visual startup confirmation: toggle LED 3 times (active LOW on PC13) */
    for (uint8_t i = 0; i < 3; i++)
    {
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_RESET); /* LED ON */
        HAL_Delay(150);
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);   /* LED OFF */
        HAL_Delay(150);
    }

    Debug_SendString("[READY] Sensor node operational\r\n");
    Debug_SendString("[READY] Type characters to test echo...\r\n\r\n");

    /* Arm first UART2 interrupt-driven reception */
    if (HAL_UART_Receive_IT(&huart2, &rx_byte, 1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Timing tracking variables for non-blocking main loop */
    uint32_t last_led_toggle_ms = HAL_GetTick();
    uint32_t last_heartbeat_ms  = HAL_GetTick();
    uint32_t heartbeat_counter  = 0;

    /* Infinite Application Loop ---------------------------------------------*/
    while (1)
    {
        uint32_t current_time_ms = HAL_GetTick();

        /* Task 1: Non-blocking LED toggle every 500 ms */
        if ((current_time_ms - last_led_toggle_ms) >= 500U)
        {
            last_led_toggle_ms = current_time_ms;
            HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        }

        /* Task 2: Non-blocking heartbeat message every 2000 ms (2 seconds) */
        if ((current_time_ms - last_heartbeat_ms) >= 2000U)
        {
            last_heartbeat_ms = current_time_ms;
            heartbeat_counter++;
            Debug_SendFormat("[HB] Heartbeat #%lu | Uptime: %lus\r\n",
                             (unsigned long)heartbeat_counter,
                             (unsigned long)(current_time_ms / 1000U));
        }
    }
}

/**
  * @brief System Clock Configuration
  *        System Clock source            = PLL (HSE)
  *        SYSCLK(Hz)                     = 100000000 (100 MHz)
  *        HCLK(Hz)                       = 100000000 (100 MHz)
  *        AHB Prescaler                  = 1
  *        APB1 Prescaler                 = 2 (50 MHz)
  *        APB2 Prescaler                 = 1 (100 MHz)
  *        HSE Frequency(Hz)              = 25000000 (25 MHz)
  *        PLL_M                          = 25
  *        PLL_N                          = 200
  *        PLL_P                          = 2
  *        PLL_Q                          = 4
  *        VDD(V)                         = 3.3
  *        Main regulator output voltage  = Scale1 mode
  *        Flash Latency(WS)              = 3 wait states
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Voltage scaling scale 1 is required for 100 MHz operation */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initialize HSE Oscillator and configure PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 25;
    RCC_OscInitStruct.PLL.PLLN       = 200;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initialize CPU, AHB and APB buses clocks with 3 wait states */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  *        Enables GPIOA, GPIOB, GPIOC clocks and configures PC13 LED output.
  * @retval None
  */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable peripheral clock for GPIO ports A, B, and C */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pin Output Level (PC13 active LOW: default HIGH = OFF) */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, GPIO_PIN_SET);

    /* Configure PC13 as Push-Pull Output */
    GPIO_InitStruct.Pin   = LED_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief USART1 Initialization Function
  *        Configured for inter-node communication (to gateway) on PA9/PA10.
  *        115200 baud, 8N1, no hardware flow control.
  * @retval None
  */
void MX_USART1_UART_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief USART2 Initialization Function
  *        Configured for PC/Debug communication on PA2/PA3.
  *        115200 baud, 8N1, RX interrupt enabled.
  * @retval None
  */
void MX_USART2_UART_Init(void)
{
    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief I2C1 Initialization Function
  *        Configured for sensor interface on PB6 (SCL) and PB7 (SDA).
  *        400kHz Fast Mode, 7-bit addressing.
  * @retval None
  */
void MX_I2C1_Init(void)
{
    hi2c1.Instance             = I2C1;
    hi2c1.Init.ClockSpeed      = 400000;
    hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1     = 0;
    hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2     = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  UART MSP Initialization
  *         Configures hardware GPIO pins, clocks, and NVIC interrupts for UART.
  * @param  huart: Pointer to UART_HandleTypeDef
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (huart->Instance == USART1)
    {
        /* Enable USART1 peripheral and GPIOA clocks */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**USART1 GPIO Configuration
           PA9  ------> USART1_TX
           PA10 ------> USART1_RX
        */
        GPIO_InitStruct.Pin       = USART1_TX_PIN | USART1_RX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (huart->Instance == USART2)
    {
        /* Enable USART2 peripheral and GPIOA clocks */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**USART2 GPIO Configuration
           PA2  ------> USART2_TX
           PA3  ------> USART2_RX
        */
        GPIO_InitStruct.Pin       = USART2_TX_PIN | USART2_RX_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure USART2 NVIC Interrupt */
        HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

/**
  * @brief  UART MSP De-Initialization
  * @param  huart: Pointer to UART_HandleTypeDef
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, USART1_TX_PIN | USART1_RX_PIN);
    }
    else if (huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, USART2_TX_PIN | USART2_RX_PIN);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }
}

/**
  * @brief  I2C MSP Initialization
  *         Configures hardware GPIO pins and clock for I2C1.
  * @param  hi2c: Pointer to I2C_HandleTypeDef
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hi2c->Instance == I2C1)
    {
        /* Enable I2C1 peripheral and GPIOB clocks */
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /**I2C1 GPIO Configuration
           PB6 ------> I2C1_SCL
           PB7 ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin       = I2C1_SCL_PIN | I2C1_SDA_PIN;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
  * @brief  I2C MSP De-Initialization
  * @param  hi2c: Pointer to I2C_HandleTypeDef
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        __HAL_RCC_I2C1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, I2C1_SCL_PIN | I2C1_SDA_PIN);
    }
}

/**
  * @brief  UART Rx Transfer completed callback.
  *         Echoes received byte back to sender and re-arms interrupt reception.
  * @param  huart: Pointer to UART_HandleTypeDef
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        /* Echo back the received byte to debug terminal */
        HAL_UART_Transmit(&huart2, &rx_byte, 1, 100);

        /* Re-arm interrupt receive for next incoming byte */
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

/**
  * @brief  UART Error callback to recover from overrun or framing errors.
  * @param  huart: Pointer to UART_HandleTypeDef
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        /* Clear error flags to prevent driver lockout */
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);

        /* Re-arm reception */
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

/**
  * @brief  Error Handler called when an unrecoverable fault occurs.
  *         Disables interrupts and executes a fast LED blink sequence.
  * @retval None
  */
void Error_Handler(void)
{
    /* Disable interrupts to halt system processing */
    __disable_irq();

    /* Fast blinking loop for visual fault indication */
    while (1)
    {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
        for (volatile uint32_t delay_count = 0; delay_count < 400000; delay_count++)
        {
            __NOP();
        }
    }
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
    /* User can add his own implementation to report the file name and line number */
    (void)file;
    (void)line;
    Error_Handler();
}
#endif /* USE_FULL_ASSERT */
