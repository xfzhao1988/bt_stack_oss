//*****************************************************************************
//
//! @file pwr_iom_utils.h
//!
//! utilities used to setup IOM pins
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
#ifndef PWR_IOM_UTILS_H
#define PWR_IOM_UTILS_H

#include "am_mcu_apollo.h"

//
//! @brief each pin can be setup as normal(spi/i2c mode), off, hi output, low output
//
typedef enum
{
    IOM_PIN_OFF,
    IOM_PIN_NOMRAL,
    IOM_PIN_GPIO_LO,
    IOM_PIN_GPIO_HI,
}
iom_pin_out_mode_e;

//
//! @brief This struct contains the IOM number and the operating mode for each IOM pin
//! and whether the IOM is SPI or I2C.
//
typedef struct
{
    uint32_t ui32IomNumber;
    iom_pin_out_mode_e eSCKPin;
    iom_pin_out_mode_e eMisoPin;
    iom_pin_out_mode_e eMosiPin;
    iom_pin_out_mode_e eCSPin;
    am_hal_iom_mode_e  eIomMode;       //<! SPI or I2C
}
iom_pins_setup_t;


// ****************************************************************************
//
//! @brief init the IOM pins depending on selected configuration
//!
//! @param psIOM_cfg  pointer to iom pin setup strcture
//!
//! @return
//
// ****************************************************************************
uint32_t pwr_iom_gpio_setup(const iom_pins_setup_t *psIOM_cfg);

// ****************************************************************************
//! @brief This will retrive the debug pin setups for this module
//! These are the pins used for GPIO debug tracking
//!
//! @param psIOM_cfg  - pointer to iom config, the dbug pins may vary depending on
//!                     IOM used
//! @param pui32DbgPinArray     - The debug pin (pin numbers) map. This is an array
//! @param ui32MaxDebugPins    - The number of pins to puopulate in the array
//!
//! @return always returns HAL standard success (0)
// ****************************************************************************
uint32_t
pwr_iom_getDebugPins(const iom_pins_setup_t *psIOM_cfg,
                     uint32_t *pui32DbgPinArray,
                     uint32_t ui32MaxDebugPins);



#endif // PWR_IOM_UTILS_H

