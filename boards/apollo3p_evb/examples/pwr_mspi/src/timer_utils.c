//*****************************************************************************
//
//! @file timer_utils.c
//!
//! @brief timer_utils
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

static tmr_callback_fcn_t callbackFcn;
void
am_ctimer_isr(void)
{
    //
    // Clear TimerA0 Interrupt.
    //
    am_hal_ctimer_int_clear(AM_HAL_CTIMER_INT_TIMERA0);

    if (callbackFcn)
    {
        callbackFcn(0);
    }
}

uint32_t
timer_init(uint32_t ui32TimerCounts, tmr_callback_fcn_t callbackFcnParam)
{
    //
    // Start a timer to trigger the ADC periodically. This timer won't actually
    // be connected to the ADC (as can be done with Timer 3). Instead, we'll
    // generate interrupts to the CPU, and then use the CPU to trigger the ADC
    // in the CTIMER interrupt handler.
    //

    callbackFcn = callbackFcnParam;
    am_hal_ctimer_config_single(0, AM_HAL_CTIMER_TIMERA,
                                AM_HAL_CTIMER_LFRC_512HZ |
                                AM_HAL_CTIMER_FN_REPEAT |
                                AM_HAL_CTIMER_INT_ENABLE);

    am_hal_ctimer_int_enable(AM_HAL_CTIMER_INT_TIMERA0);

    am_hal_ctimer_period_set(0, AM_HAL_CTIMER_TIMERA, ui32TimerCounts, 0);

    //
    // Start the timer.
    //
    am_hal_ctimer_start(0, AM_HAL_CTIMER_TIMERA);

    NVIC_EnableIRQ(CTIMER_IRQn);

    return 0;
}

void
timer_disable(void)
{

    NVIC_DisableIRQ(CTIMER_IRQn);
    am_hal_ctimer_stop(0, AM_HAL_CTIMER_TIMERA);

}

