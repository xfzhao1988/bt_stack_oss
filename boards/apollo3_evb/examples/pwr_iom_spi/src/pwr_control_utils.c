//*****************************************************************************
//
//! @file pwr_pwrControl_utils.c
//!
//! @brief Set power mode(s) for the example
//!
//! Contains utilities to manage device power modes
//!
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
#include "pwr_control_utils.h"
#include "am_bsp.h"
#include "pwr_common_error_definitions.h"

//
//! random number table
//
static const uint8_t g_random[] =
{
    0x15, 0x2f, 0x8f, 0xdf, 0xc3, 0xa0, 0x6c, 0x16, 0x19, 0xa5, 0x18, 0xc1, 0x18, 0x65, 0xa8, 0x44,
    0xd2, 0x53, 0x3b, 0x14, 0xb0, 0x59, 0xae, 0x00, 0x67, 0xf0, 0x6e, 0x30, 0x35, 0x21, 0x50, 0x02,
    0xf1, 0x5b, 0x1c, 0xcc, 0x4d, 0x1e, 0xd1, 0x5c, 0x30, 0xea, 0xdf, 0xf9, 0xf6, 0x39, 0x06, 0x78,
    0xd1, 0xf3, 0xef, 0x79, 0xc8, 0x34, 0x95, 0xe5, 0x38, 0x2a, 0x75, 0xb4, 0xc9, 0x76, 0x58, 0xc3,
    0xe0, 0x7f, 0x7e, 0x82, 0xa0, 0xd0, 0xac, 0x02, 0x43, 0x2d, 0xdc, 0x3d, 0x45, 0xb2, 0x97, 0x82,
    0xb5, 0x7f, 0x9c, 0x5a, 0xcf, 0x46, 0x81, 0xd4, 0x4d, 0xf9, 0xa7, 0xcc, 0xcb, 0x6e, 0xb3, 0x1d,
    0x09, 0x04, 0x6d, 0x36, 0x57, 0xc0, 0x52, 0xa6, 0x8a, 0x0c, 0xd5, 0x66, 0xeb, 0x56, 0xaa, 0x68,
    0xec, 0x25, 0x78, 0x01, 0x88, 0x63, 0x7b, 0xbf, 0xe2, 0x01, 0xbf, 0xed, 0xc5, 0x9a, 0x3c, 0xbb,
    0x45, 0x2f, 0xac, 0x8e, 0x4e, 0x57, 0x10, 0x0c, 0x29, 0x4d, 0xfe, 0xd2, 0xd6, 0x2b, 0x48, 0xad,
    0x06, 0xef, 0x51, 0xd6, 0x95, 0xa4, 0xb6, 0xe2, 0x5a, 0x30, 0x63, 0x56, 0xcd, 0x28, 0x52, 0xf6,
    0x60, 0x79, 0xa3, 0x71, 0xde, 0x02, 0xf5, 0x63, 0x5c, 0xe6, 0xe3, 0x59, 0x71, 0x7a, 0xfa, 0xd9,
    0x9d, 0x45, 0x25, 0xbc, 0xe7, 0xbe, 0x84, 0xf7, 0x48, 0xf5, 0x60, 0x37, 0x72, 0x59, 0xc6, 0xbb,
    0x22, 0xa4, 0xdd, 0xb0, 0xcb, 0x73, 0x84, 0xed, 0xa2, 0x74, 0xb6, 0x94, 0x01, 0x20, 0xcb, 0xa4,
    0xf5, 0x9d, 0xab, 0x20, 0xb8, 0xff, 0x07, 0x71, 0x51, 0xee, 0x9b, 0x08, 0x4a, 0xd8, 0x81, 0x31,
    0xd7, 0xfb, 0xa9, 0xef, 0x93, 0x13, 0x7c, 0xc6, 0xb8, 0x13, 0x53, 0x83, 0x49, 0xd9, 0xc3, 0x84,
    0xe6, 0x54, 0x43, 0x54, 0x24, 0xea, 0x32, 0xbb, 0x62, 0x4d, 0x08, 0x0c, 0x70, 0x43, 0xc5, 0xcb,
};

static const am_hal_cachectrl_config_t am_hal_cachectrl_benchmark =
{
    .bLRU                       = 0,
    .eDescript                  = AM_HAL_CACHECTRL_DESCR_1WAY_128B_512E,
    .eMode                      = AM_HAL_CACHECTRL_CONFIG_MODE_INSTR,
};

//*****************************************************************************
//
//! @brief set the power modes for this example
//
//*****************************************************************************
uint32_t
pwrctrl_set_low_power(void)
{
    uint32_t ui32Status;
    do
    {
        //
        // Set the default cache configuration
        //
        ui32Status = am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
        ERROR_MACRO(ui32Status, ePwrCntrlCacheConfig)

        am_hal_cachectrl_enable();

        //
        // Configure the board for low power operation.
        //
        am_bsp_low_power_init();

        //
        // Enable only the first 512KB bank of Flash (0).  Disable Flash(1)
        //
        ui32Status = am_hal_pwrctrl_memory_enable(AM_HAL_PWRCTRL_MEM_FLASH_MIN);
        ERROR_MACRO(ui32Status, ePwrCntrlPwrPeriphDis)
    }
    while (false);

    return ui32Status;
}
//*****************************************************************************
//
// fill a buffer incrementing bytes
//
//*****************************************************************************
void
pwrctrl_fill_buff_incrementing_bytes(uint8_t *pui8Array, int32_t i32Size)
{

    uint8_t bytePattern = 0;
    while (i32Size-- > 0)
    {
        *pui8Array++ = bytePattern++;
    }

}

//*****************************************************************************
//
// fill a buffer with altenating bits and bytes or bits and halfwords
//
//*****************************************************************************
void
pwrctrl_fill_buff_alternating_bits(uint8_t *pui8Array,
                                   uint32_t ui32TransferBitWidth,
                                   int32_t i32Size)
{
    //
    // Alternating bytes
    //
    uint32_t ui32FillPattern = 0xA55AA55A;
    if ( ui32TransferBitWidth > 8)
    {
        //
        // Alternating shorts (16 bit chunks) for MSPI 16-bit-bus-width transfers)
        //
        ui32FillPattern = 0xA55A5AA5;
    }
    //
    // write a 32-bit chunk at a time
    //
    uint32_t *pui32p = (uint32_t *)pui8Array;
    i32Size >>= 2;  // get num longwords, div by 4
    while(i32Size-- > 0)
    {
        *pui32p++ = ui32FillPattern;
    }
}
//*****************************************************************************
//
// fill a buffer with data using the (fixed) random array
//
//*****************************************************************************
void
pwrctrl_fill_buff_random(uint8_t *pui8Array, int32_t i32Size)
{

    if (i32Size > 0)
    {
        uint32_t ui32RandomSize = sizeof(g_random);
        uint32_t randBuffIdx = 0;
        while (--i32Size >= 0)
        {
            *pui8Array++ = g_random[randBuffIdx];
            if (++randBuffIdx >= ui32RandomSize)
            {
                randBuffIdx = 0;
            }
        }
    }

}


