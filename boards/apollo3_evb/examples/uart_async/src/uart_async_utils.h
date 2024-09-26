//*****************************************************************************
//
//! @file uart_async_utils.h
//!
//! @brief Functions to setup and manage the uart asynchronous driver
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2024, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk_3_2_0-dd5f40c14b of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef RTT_UART_ASYNC_UTILS_H
#define RTT_UART_ASYNC_UTILS_H

#include "am_mcu_apollo.h"
#include "am_bsp.h"


#define UART_ID                            (0)
#define TIMER_NUM       0               ///< Timer number used in the example
#define MAX_UART_PACKET_SIZE            (2048)
#define UART_RX_TIMEOUT_MS              (5)
#define NUM_UARTS_USED 1

//
//! enumerate uart names
//
typedef enum
{
#if defined(AM_BSP_GPIO_UART_TX) && defined(AM_BSP_GPIO_UART_RX)
    eUART0,
#endif
#if defined(AM_BSP_GPIO_UART1_TX) && defined(AM_BSP_GPIO_UART1_RX)
    eUART1,
#endif
    eMAX_UARTS,
    euidX32 = 0x70000000,  // force this to 32bit value on all compilers
}
uart_id_e;

//
//! define data used per uart instance
//
typedef struct
{
    void *pvUART;
    uint32_t ui32ISrErrorCount;
    am_hal_uart_status_t e32Status;

    //
    //! Saved buffer sizes for the uart
    //
    uint32_t ui32TxBuffSize;
    uint32_t ui32RxBuffSize;

    //
    //! in this example, all uart buffers are the same size,
    //! this is not needed and can be modified
    //
    uint8_t pui8UARTTXBuffer[MAX_UART_PACKET_SIZE];
    uint8_t pui8UARTRXBuffer[MAX_UART_PACKET_SIZE];

    uart_id_e uartId;
    volatile bool bRxComplete;

}
uart_async_local_vars_t;

uint32_t init_uart(uart_id_e uartId, uart_async_local_vars_t **ppUartLocalVar);

#endif //RTT_UART_ASYNC_UTILS_H

