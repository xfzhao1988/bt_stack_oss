//*****************************************************************************
//
//! @file timer_utils.c
//!
//! @brief timer untilties. Creates a timed callback
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
#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"

#define LOCAL_IRQ_PRIORITY_DEFAULT 0x04

#define PDM_IRQ_PRIORITY 0x03
#define PDM_TIMER_PRIORITY LOCAL_IRQ_PRIORITY_DEFAULT
#define TMR_TICKS_PER_SECOND 512

//
//! prototye for timer callback
//
typedef void (*tmr_callback_fcn_t)(void *x);

//*****************************************************************************
//
//! @brief enable the periodic timer interrupt
//!
//! @param ui32LfrcCounts   // number of timer counts used, use TMR_TICKS_PER_SECOND as reference
//! @param callbackFcnParam // function called from timer isr when timer expires
//!
//! @return standard hal status
//
//*****************************************************************************
extern uint32_t timer_init(uint32_t ui32LfrcCounts, am_hal_ctimer_handler_t callbackFcnParam);

//*****************************************************************************
//
//! @brief disable the timer
//
//*****************************************************************************
extern void timer_disable(void);

#endif //TIMER_UTILS_H

