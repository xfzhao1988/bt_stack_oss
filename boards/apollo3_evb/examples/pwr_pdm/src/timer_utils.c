//*****************************************************************************
//
//! @file timer_utils.c
//!
//! @brief
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

#include "timer_utils.h"

//
//! saved timer callback function pointer
//
#define CTIMER_NUMBER 0
#define CTIMER_INTERRUPT AM_HAL_CTIMER_INT_TIMERA0

//*****************************************************************************
//
// timer ISR
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    //
    // Clear TimerA0 Interrupt.
    //
    uint32_t ui32Status = am_hal_ctimer_int_status_get(true);
    am_hal_ctimer_int_clear(CTIMER_INTERRUPT);
    am_hal_ctimer_int_service(ui32Status);
}

//*****************************************************************************
//
// timer init function
//
//*****************************************************************************
uint32_t
timer_init(uint32_t ui32LfrcCounts, am_hal_ctimer_handler_t callbackFcnParam)
{
    am_hal_ctimer_config_single(CTIMER_NUMBER, AM_HAL_CTIMER_TIMERA,
                                AM_HAL_CTIMER_LFRC_512HZ |
                                AM_HAL_CTIMER_FN_REPEAT |
                                AM_HAL_CTIMER_INT_ENABLE);

    am_hal_ctimer_int_enable(CTIMER_INTERRUPT);

    am_hal_ctimer_period_set(CTIMER_NUMBER, AM_HAL_CTIMER_TIMERA, ui32LfrcCounts, 0);

    am_hal_ctimer_int_register(CTIMER_INTERRUPT, callbackFcnParam);

    //
    // Start the timer.
    //
    am_hal_ctimer_start(CTIMER_NUMBER, AM_HAL_CTIMER_TIMERA);

    NVIC_SetPriority(CTIMER_IRQn, LOCAL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(CTIMER_IRQn);

    NVIC_EnableIRQ(CTIMER_IRQn);

    return 0;
}
//*****************************************************************************
//
// disable timer
//
//*****************************************************************************

void
timer_disable(void)
{

    NVIC_DisableIRQ(CTIMER_IRQn);
    am_hal_ctimer_stop(CTIMER_NUMBER, AM_HAL_CTIMER_TIMERA);
}

