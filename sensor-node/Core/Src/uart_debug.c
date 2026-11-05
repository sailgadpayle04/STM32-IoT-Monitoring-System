/**
  ******************************************************************************
  * @file           : uart_debug.c
  * @brief          : Implementation of UART debug utility functions.
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
#include "uart_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
static UART_HandleTypeDef *s_debug_uart = NULL;
static char s_format_buffer[DEBUG_FORMAT_BUFFER_SIZE];

/* Exported Functions --------------------------------------------------------*/

/**
  * @brief  Initialize the debug module with a UART handle.
  * @param  huart: Pointer to the UART peripheral handle configured for debug.
  */
void Debug_Init(UART_HandleTypeDef *huart)
{
    s_debug_uart = huart;
}

/**
  * @brief  Send a null-terminated string over the debug UART.
  * @param  str: Null-terminated string to transmit.
  */
void Debug_SendString(const char *str)
{
    if ((s_debug_uart == NULL) || (str == NULL))
    {
        return;
    }

    uint16_t len = (uint16_t)strlen(str);
    if (len > 0)
    {
        HAL_UART_Transmit(s_debug_uart, (const uint8_t *)str, len, DEBUG_UART_TIMEOUT_MS);
    }
}

/**
  * @brief  Send formatted text via UART using vsnprintf.
  * @param  fmt: Format string.
  * @param  ...: Variable arguments list.
  */
void Debug_SendFormat(const char *fmt, ...)
{
    if ((s_debug_uart == NULL) || (fmt == NULL))
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(s_format_buffer, sizeof(s_format_buffer), fmt, args);
    va_end(args);

    if (written > 0)
    {
        uint16_t len = (uint16_t)((written < (int)sizeof(s_format_buffer)) ? written : (sizeof(s_format_buffer) - 1));
        HAL_UART_Transmit(s_debug_uart, (const uint8_t *)s_format_buffer, len, DEBUG_UART_TIMEOUT_MS);
    }
}

/**
  * @brief  Send raw bytes over the debug UART.
  * @param  data: Pointer to data buffer.
  * @param  len: Length in bytes.
  */
void Debug_SendBytes(const uint8_t *data, uint16_t len)
{
    if ((s_debug_uart == NULL) || (data == NULL) || (len == 0))
    {
        return;
    }

    HAL_UART_Transmit(s_debug_uart, (const uint8_t *)data, len, DEBUG_UART_TIMEOUT_MS);
}
