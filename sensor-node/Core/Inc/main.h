/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines and function
  *                   prototypes for the sensor node application.
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

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/* Exported Types ------------------------------------------------------------*/
/* Exported Constants --------------------------------------------------------*/

/* Hardware Pin Definitions --------------------------------------------------*/

/* On-board User LED (active LOW on most STM32F411 BlackPill boards) */
#define LED_PIN                     GPIO_PIN_13
#define LED_GPIO_PORT               GPIOC

/* USART2 - Debug / Host PC Interface (115200 8N1) */
#define USART2_TX_PIN               GPIO_PIN_2
#define USART2_TX_PORT              GPIOA
#define USART2_RX_PIN               GPIO_PIN_3
#define USART2_RX_PORT              GPIOA

/* USART1 - Inter-Node Communication to Gateway (115200 8N1) */
#define USART1_TX_PIN               GPIO_PIN_9
#define USART1_TX_PORT              GPIOA
#define USART1_RX_PIN               GPIO_PIN_10
#define USART1_RX_PORT              GPIOA

/* I2C1 - Sensor Bus (PB6: SCL, PB7: SDA, 400kHz Fast Mode) */
#define I2C1_SCL_PIN                GPIO_PIN_6
#define I2C1_SCL_PORT               GPIOB
#define I2C1_SDA_PIN                GPIO_PIN_7
#define I2C1_SDA_PORT               GPIOB

/* Exported Macros -----------------------------------------------------------*/

/* Exported Variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef  hi2c1;

/* Exported Functions Prototypes ---------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

void MX_GPIO_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_I2C1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
