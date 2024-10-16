//*****************************************************************************
//
//! @file pwr_iom_utils.c
//!
//! @brief Utilities used to setup IOM pins
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
#include "pwr_iom_utils.h"
#include "am_bsp.h"
#include "pwr_gpio_utils.h"

#define MAX_DEBUG_PINS 3

//
//! pin config for GPIO debug output pins
//
static const am_hal_gpio_pincfg_t g_AM_GPIO_OFF =
    {
        .uFuncSel         = 3,
        .eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        //.ePullup          = AM_HAL_GPIO_PIN_PULLUP_NONE,
        .eGPInput         = AM_HAL_GPIO_PIN_INPUT_NONE,
        //.eGPRdZero        = AM_HAL_GPIO_PIN_RDZERO_READPIN,
        .eIntDir          = AM_HAL_GPIO_PIN_INTDIR_NONE,
        .eGPOutcfg        = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
    };

//
//! pin config for GPIO debug output pins
//
static const am_hal_gpio_pincfg_t g_AM_GPIO_OUTPUT_CONFIG =
{
    .uFuncSel         = 3,
    .eDriveStrength   = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPInput         = AM_HAL_GPIO_PIN_INPUT_NONE,
    .eIntDir          = AM_HAL_GPIO_PIN_INTDIR_LO2HI,
    .eGPOutcfg        = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
};


typedef struct
{
    uint32_t pinNumber;
    const am_hal_gpio_pincfg_t *pinCfg;
} pin_defs_t;

typedef struct
{
    pin_defs_t sck;
    pin_defs_t miso;
    pin_defs_t mosi;
    pin_defs_t cs;
    uint32_t  ui32DebugPin[MAX_DEBUG_PINS]; //!< Array of pin numbers used for debug, see dbg_pin_names_e
} iom_defs_t;


const iom_defs_t g_iomDefsTable[AM_REG_IOM_NUM_MODULES] =
{
    [0] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM0_SCK,   // 5
        .sck.pinCfg =  &g_AM_BSP_GPIO_IOM0_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM0_MISO,  // 7
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM0_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM0_MOSI,  // 6
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM0_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM0_CS,  // 50
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM0_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = DEBUG_PIN_RD_LO,
        .ui32DebugPin[eDEBUG_PIN_2] = DEBUG_PIN_2,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = DEBUG_PIN_SLEEP
    },
    [1] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM1_SCK,  //  8
        .sck.pinCfg = &g_AM_BSP_GPIO_IOM1_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM1_MISO,  // 10
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM1_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM1_MOSI,  // 9
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM1_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM1_CS,  // 51
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM1_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = DEBUG_PIN_RD_LO,
        .ui32DebugPin[eDEBUG_PIN_2] = DEBUG_PIN_2,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = DEBUG_PIN_SLEEP
    },
#ifdef AM_BSP_GPIO_IOM2_SCK
    [2] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM2_SCK,  // 25
        .sck.pinCfg = &g_AM_BSP_GPIO_IOM2_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM2_MISO,  // 27
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM2_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM2_MOSI,  // 26
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM2_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM2_CS,  // 52
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM2_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = 0,
        .ui32DebugPin[eDEBUG_PIN_2] = 3,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = 2
    },
#endif
    [3] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM3_SCK,  // 31
        .sck.pinCfg = &g_AM_BSP_GPIO_IOM3_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM3_MISO,  // 33
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM3_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM3_MOSI,  // 32
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM3_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM3_CS,  // 90
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM3_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = DEBUG_PIN_RD_LO,
        .ui32DebugPin[eDEBUG_PIN_2] = DEBUG_PIN_2,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = DEBUG_PIN_SLEEP
    },
    [4] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM4_SCK,  // 34
        .sck.pinCfg = &g_AM_BSP_GPIO_IOM4_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM4_MISO,  // 36
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM4_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM4_MOSI,  // 35
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM4_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM4_CS,  // 54
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM4_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = DEBUG_PIN_RD_LO,
        .ui32DebugPin[eDEBUG_PIN_2] = DEBUG_PIN_2,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = DEBUG_PIN_SLEEP
    },
    [5] =
    {
        .sck.pinNumber = AM_BSP_GPIO_IOM5_SCK,  // 47
        .sck.pinCfg = &g_AM_BSP_GPIO_IOM5_SCK,
        .miso.pinNumber = AM_BSP_GPIO_IOM5_MISO,  // 49
        .miso.pinCfg = &g_AM_BSP_GPIO_IOM5_MISO,
        .mosi.pinNumber = AM_BSP_GPIO_IOM5_MOSI,  // 48
        .mosi.pinCfg = &g_AM_BSP_GPIO_IOM5_MOSI,
        .cs.pinNumber   = AM_BSP_GPIO_IOM5_CS,  // 92
        .cs.pinCfg = &g_AM_BSP_GPIO_IOM5_CS,
        .ui32DebugPin[eDEBUG_PIN_RD_LOW_WRT_HI] = DEBUG_PIN_RD_LO,
        .ui32DebugPin[eDEBUG_PIN_2] = DEBUG_PIN_2,
        .ui32DebugPin[eDEBUG_PIN_SLEEP] = DEBUG_PIN_SLEEP
    },

};

static uint32_t pwr_iom_MOSI_MISO_setup(const pin_defs_t  *ePinDefs,
                                        iom_pin_out_mode_e ePinMode);

//*****************************************************************************
//
//! @brief enable/disable an IOM pin
//!
//! @param ePinDefs  definitions for this pin from  iomDefsTable
//! @param ePinMode pin mode selected by calling program
//!
//! @return standard HAL status
//
//*****************************************************************************
static uint32_t
pwr_iom_MOSI_MISO_setup( const pin_defs_t  *ePinDefs, iom_pin_out_mode_e ePinMode )
{
    am_hal_gpio_pincfg_t pinCfg;
    switch(ePinMode)
    {
        case IOM_PIN_OFF:
            pinCfg = g_AM_GPIO_OFF;
            break;
        case IOM_PIN_NOMRAL:
            pinCfg = *ePinDefs->pinCfg;
            break;
        case IOM_PIN_GPIO_LO:
        case IOM_PIN_GPIO_HI:
            pinCfg = g_AM_GPIO_OUTPUT_CONFIG;
            break;
    }

    uint32_t ui32PinNUm = ePinDefs->pinNumber;
    uint32_t ui32Stat = am_hal_gpio_pinconfig( ui32PinNUm , pinCfg);
    if (ui32Stat)
    {
        return ui32Stat;
    }

    switch( ePinMode)
    {
        case IOM_PIN_OFF:
        case IOM_PIN_NOMRAL:
            //
            // nothing more to do here, the iom will control the pin
            //
            break;
        case IOM_PIN_GPIO_LO:
            am_hal_gpio_output_clear(ui32PinNUm);
            break;
        case IOM_PIN_GPIO_HI:
            am_hal_gpio_output_set(ui32PinNUm);
            break;
    }

    return AM_HAL_STATUS_SUCCESS;

}

//*****************************************************************************
//
// Init the IOM pins depending based on pin config preferences
//
//*****************************************************************************
uint32_t
pwr_iom_gpio_setup (const iom_pins_setup_t *psIOM_cfg)
{
    uint32_t ui32IOM_num = psIOM_cfg->ui32IomNumber;

    //
    // get the pin usage map from the table above for each specific IOM
    //
    const iom_defs_t *piomd = &g_iomDefsTable[ui32IOM_num];

    //
    // setup each pin, based on the pin map (piomd) and the
    // pre-configured pin mode (ptIOM_cfg)
    //
    pwr_iom_MOSI_MISO_setup(&piomd->sck,  psIOM_cfg->eSCKPin);
    pwr_iom_MOSI_MISO_setup(&piomd->miso, psIOM_cfg->eMisoPin);
    pwr_iom_MOSI_MISO_setup(&piomd->mosi, psIOM_cfg->eMosiPin);
    pwr_iom_MOSI_MISO_setup(&piomd->cs,   psIOM_cfg->eCSPin);

    return AM_HAL_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Populate the debug pin number array
//
//*****************************************************************************
uint32_t
pwr_iom_getDebugPins (const iom_pins_setup_t *psIOM_cfg,
                      uint32_t *pui32DbgPinArray,
                      uint32_t ui32MaxDebugPins)
{
    uint32_t ui32IOM_num = psIOM_cfg->ui32IomNumber;
    const uint32_t  *pUI32DebugPinTable = g_iomDefsTable[ui32IOM_num].ui32DebugPin;
    for (uint32_t i = 0; i < ui32MaxDebugPins; i++)
    {
        pui32DbgPinArray[i] = *pUI32DebugPinTable++;
    }

    return AM_HAL_STATUS_SUCCESS;
}

