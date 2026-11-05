/**
  ******************************************************************************
  * @file           : uart_debug.h
  * @brief          : Header file for debug UART output utility.
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

#ifndef __UART_DEBUG_H
#define __UART_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/* Exported Constants --------------------------------------------------------*/
#define DEBUG_UART_TIMEOUT_MS       1000U
#define DEBUG_FORMAT_BUFFER_SIZE    256U

/* Exported Functions Prototypes ---------------------------------------------*/

/**
  * @brief  Initialize the debug UART module with the given UART peripheral handle.
  * @param  huart: Pointer to UART_HandleTypeDef for the debug port (e.g., huart2)
  * @retval None
  */
void Debug_Init(UART_HandleTypeDef *huart);

/**
  * @brief  Transmit a null-terminated string over the debug UART.
  * @param  str: Pointer to null-terminated C string
  * @retval None
  */
void Debug_SendString(const char *str);

/**
  * @brief  Transmit formatted text (printf-style) over the debug UART.
  * @param  fmt: Format string followed by variable arguments
  * @retval None
  */
void Debug_SendFormat(const char *fmt, ...);

/**
  * @brief  Transmit raw binary bytes over the debug UART.
  * @param  data: Pointer to data buffer
  * @param  len: Number of bytes to transmit
  * @retval None
  */
void Debug_SendBytes(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __UART_DEBUG_H */
