//*****************************************************************************
//
//! @file pwr_pdm.c
//!
//! @brief Example for PWM power measurements.
//!
//! @addtogroup examples Examples
//!
//! @defgroup pwr_pdm PDM Power Example
//! @ingroup examples
//! @{
//!
//! Purpose: Simple example that uses HAL to perform multiple PDM transfers.
//!
//! @verbatim
//!    1. Enter deepsleep during DMA transfer.
//!    2. DMA write and read-loop every 1s.
//!    3. Have configurable PDM buffer size.
//!    4. GPIO indicator for CPU wake state.
//!    5. Run for a configurable number of loops.
//!
//! Pin connections
//! Debug:
//!     GPIO[8]  DEBUG_PIN_1
//!     GPIO[9]  DEBUG_PIN_2
//!
//! PDM Mode Description
//!    a. Mode 0  PDM not enabled
//!    b. Mode 1  PDM enable not running
//!    c. Mode 2  continuous PDM reads
//!    d. Mode 3  run a fixed transfer sequence periodically (1 Hz), then restart sequence
//! @endverbatim
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

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "timer_utils.h"
#include "pwr_gpio_utils.h"

#define PRINT_ENABLE 0

//
//! enables printing in the main loop, this is usually off for low power
//
#define PRINT_IN_LOOP 0

#if (PRINT_ENABLE==0) && (PRINT_IN_LOOP==1)
#warning "print config error"
#undef PRINT_IN_LOOP
#define PRINT_IN_LOOP 0
#endif

#if PRINT_ENABLE == 0
#define STARTUP_PRINT(args...)
#else
#define STARTUP_PRINT(args...) am_util_stdio_printf(args)
#endif

#if PRINT_IN_LOOP == 0
#define DEBUG_PRINT(args...)
#else
#define DEBUG_PRINT(args...) am_util_stdio_printf(args)
#endif
#define FIFO_THRESHOLD_CNT      16

//
//! @brief
//!
//!   - PDM_MODE 0  no pdm enabled
//!   - PDM_MODE 1  pdm inited not used
//!   - PDM_MODE 2  pdm continuous
//!   - PDM_MODE 3  pdm sequence
//

#define PDM_MODE_NO_PDM 0  // deepsleep only mode
#define PDM_INIT_ONLY   1
#define PDM_MODE_CONTINUOUS 2
#define PDM_MODE_SEQUENCE_TIMED_TX 3

#define PDM_MODE (PDM_MODE_SEQUENCE_TIMED_TX)

#define TESTDATASIZE 4096

#define BUFFERTEST_FILL 0


//
//! @name DebugPins\n
//! Debug Pin Setups
//! @{
//!
//! when set to 1 the debug pins are enabled
//!

//
//! Define debug GPIO pins, these are used only when GPIO_DEBUG_ENABLE == 1\n
//!@{
//!
#define GPIO_DEBUG_ENABLE 0

//!
//! Debug Pin 1:
//!     - This toggles when a new PDM DMA transfer is started.\n
//!
#define DEBUG_DMA_XFR_TOGGLE_PIN_NUM_1 5  //!< eDEBUG_PDM_XFR_TOGGLE

//!
//! Debug Pin 2 is active high for each individual sequence
//!
#define DEBUG_SEQ_IN_PROGRESS_PIN_NUM 6

//!
//! Debug Sleep Pin 2 is low when the processor is in DeepSleep
//!
#define DEBUG_SLEEP_PIN_NUM 7 //!< eDEBUG_SLEEP
//
//! @} Debug GPIO Pins

//! @} Debug Pin Setups

//
//! number of non-blocking (rand and write) transmission run per sequence
//
#define NUM_PDM_DMA_XFER_PER_SEQ 4
//
//! Choose iom module, take care with potential PDM and debug pin conflicts
//
#define     PDM_MODULE          0   // PDM module to select. Only PDM0 exists on Apollo3x

#define     USE_DMA             1   // 0 = use FIFO only, 1 = use DMA, Only USE_DMA = 1 is supported.

#if PRINT_ENABLE == 0
#define STARTUP_PRINT(args...)
#else
#define STARTUP_PRINT(args...) am_util_stdio_printf(args)
#endif

#if PRINT_IN_LOOP == 0
#define DEBUG_PRINT(args...)
#else
#define DEBUG_PRINT(args...) am_util_stdio_printf(args)
#endif

//*****************************************************************************
//
//! Global Variables
//! @{
//
//*****************************************************************************

//
//! Collect most of the ram variables used here in this struct
//
typedef struct
{
    void *g_PeriphHandle;
    uint32_t ui32DebugPinNum[eMAX_DEBUG_PINS];  //!< Debug pin numbers needs to be set early in startup
    uint32_t ui32HFRC2TuneCount;                //<! in units of timer period
    uint32_t ui32numPDMTransfersThisSequence;   //!< count number of transfers, compare to NUM_PDM_DMA_XFER_PER_SEQ
    uint32_t ui32NumDmaXferComplete;
    uint32_t ui32PDMIsrCount;
    uint32_t ui32DCMPIsrCount;
    uint32_t ui32FifoOVFCount;
    uint32_t ui32DMAError;
    uint32_t ui32DmaStatError;
    uint32_t ui32DelayRestartCount;
    volatile int32_t *pPDMData;

    volatile bool bTimerFlag;           //!< set (isr level) in timer callback (ISR) when timer expires
    volatile bool bPDMDataReady;        //<! set in the PDM isr when a dma transfer completes
    bool bTransferRunning;
    bool bSequenceStarted;

    am_hal_pdm_transfer_t tPDMTransfer;     //!< this needs init before use
    uint32_t ui32ReadData[TESTDATASIZE]; //!< read data here
}
globasx_t;

typedef struct
{
    uint32_t ui32TestData[TESTDATASIZE]; //!< double buffer A
    uint32_t ui32TestDataB[TESTDATASIZE]; //!< double buffer B
    uint32_t ui32TestDataC[TESTDATASIZE]; //!< extra buffer to debug DMA oversample
    uint32_t ui32PdmOverflowTest;

} glob_sram_placement_t;

//
//! allocate ram variables used
//
#ifdef AM_PART_APOLLO3P
__attribute__((section(".tcm")))
#endif
globasx_t g_tGlb;

#ifdef AM_PART_APOLLO3P
__attribute__((section("readwrite")))
#endif
__attribute__ ((aligned(32))) glob_sram_placement_t g_tDMA;

//
//! this will needed to be inited before use
//

//! @}
// end of global variables

//
// this is from the apollo3 FFT example
//! definee PDM config
//
const am_hal_pdm_config_t g_tPdmConfig =
{
    .eClkDivider = AM_HAL_PDM_MCLKDIV_1,
    .eLeftGain = AM_HAL_PDM_GAIN_0DB,
    .eRightGain = AM_HAL_PDM_GAIN_0DB,
    .ui32DecimationRate = 64,
    .bHighPassEnable = 0,
    .ui32HighPassCutoff = 0xB,
    .ePDMClkSpeed = AM_HAL_PDM_CLK_750KHZ,
    .bInvertI2SBCLK = 0,
    .ePDMClkSource = AM_HAL_PDM_INTERNAL_CLK,
    .bPDMSampleDelay = 0,
    .bDataPacking = 1,
    .ePCMChannels = AM_HAL_PDM_CHANNEL_STEREO,
    .ui32GainChangeDelay = 1,
    .bI2SEnable = 0,
    .bSoftMute = 0,
    .bLRSwap = 0,
    .bDoNotStartPdm = true,
};

//
//! define PDM pin names used
//
typedef enum
{
    PN_PDMATA,
    PN_PDMCLK,
    PN_SIGNAL,
    PN_NUM_PDM_PINS,
}
pin_names_e;

const bool g_bSequenceMode = PDM_MODE == PDM_MODE_SEQUENCE_TIMED_TX;

//
//! define PDM pins
//
const pdm_pin_defs_t gsPinCfg[PN_NUM_PDM_PINS] =
{
    [PN_PDMATA]  =
    {
        .ui32PinNumber = 11, .bInitThisPin = true, .sPinCfg.uFuncSel = AM_HAL_PIN_11_PDMDATA
    },
    [PN_PDMCLK]  =
    {
        .ui32PinNumber = 12, .bInitThisPin = true, .sPinCfg.uFuncSel = AM_HAL_PIN_12_PDMCLK
    },
    [PN_SIGNAL] =
    {
        .ui32PinNumber = 14,
        .sPinCfg.uFuncSel   = 3,
        .bSetOutput         = true,
        .bInitThisPin       = true,
        .eOutputVal = AM_HAL_GPIO_OUTPUT_CLEAR,
        .sPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        .sPinCfg.eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
    },
};

const uint32_t g_patternA = 0xFCAC0000;
const uint32_t g_patternB = 0xECBC0000;
const uint32_t g_patternC = 0x4C3C0000;

//
//! define debug pins, if debug is enabled
//
#if GPIO_DEBUG_ENABLE == 1
const pdm_pin_defs_t g_tDebugPinCfg[eMAX_DEBUG_PINS] = \
{
    [eDEBUG_PDM_XFR_TOGGLE] =
    {
        .ui32PinNumber = DEBUG_DMA_XFR_TOGGLE_PIN_NUM_1,
        .sPinCfg.uFuncSel  =  3,         // GPIO
        .bSetOutput        = true,
        .bInitThisPin      = true,
        .eOutputVal = AM_HAL_GPIO_OUTPUT_CLEAR,
        .sPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        .sPinCfg.eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
    },

    [eDEBUG_XFR_SEQUENCE] =
    {
        .ui32PinNumber = DEBUG_SEQ_IN_PROGRESS_PIN_NUM,
        .sPinCfg.uFuncSel  =  3,         // GPIO
        .bSetOutput        = true,
        .bInitThisPin      = true,
        .eOutputVal = AM_HAL_GPIO_OUTPUT_CLEAR,
        .sPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        .sPinCfg.eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
    },

    [eDEBUG_SLEEP] =
    {
        .ui32PinNumber = DEBUG_SLEEP_PIN_NUM,
        .sPinCfg.uFuncSel  =  3,         // GPIO
        .bSetOutput        = true,
        .bInitThisPin      = true,
        .eOutputVal = AM_HAL_GPIO_OUTPUT_CLEAR,
        .sPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
        .sPinCfg.eGPOutcfg      = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL
    },
};

//
//! set up debug function #defines\n
//! (disable gpio functions if debug pins aren't used)
//
const bool g_bGpioEnabled = true;
#define GPIO_DBG1_HI am_hal_gpio_output_set(g_tDebugPinCfg[eDEBUG_PDM_XFR_TOGGLE].ui32PinNumber)
#define GPIO_DBG1_LO am_hal_gpio_output_clear(g_tDebugPinCfg[eDEBUG_PDM_XFR_TOGGLE].ui32PinNumber)
#define GPIO_DBG1_TGLE am_hal_gpio_output_toggle(g_tDebugPinCfg[eDEBUG_PDM_XFR_TOGGLE].ui32PinNumber)
#define GPIO_DBG2_HI am_hal_gpio_output_set(g_tDebugPinCfg[eDEBUG_XFR_SEQUENCE].ui32PinNumber)
#define GPIO_DBG2_LO am_hal_gpio_output_clear(g_tDebugPinCfg[eDEBUG_XFR_SEQUENCE].ui32PinNumber)
#define GPIO_SLEEP_HI am_hal_gpio_output_set(g_tDebugPinCfg[eDEBUG_SLEEP].ui32PinNumber)
#define GPIO_SLEEP_LO am_hal_gpio_output_clear(g_tDebugPinCfg[eDEBUG_SLEEP].ui32PinNumber)

#else // if GPIO_DEBUG_ENABLE == 1

//! (disable gpio functions if debug pins aren't used)

const bool g_bGpioEnabled = false;
#define GPIO_DBG1_HI
#define GPIO_DBG1_LO
#define GPIO_DBG1_TGLE
#define GPIO_DBG2_HI
#define GPIO_DBG2_LO
#define GPIO_SLEEP_HI
#define GPIO_SLEEP_LO
#endif // #else if !(GPIO_DEBUG_ENABLE == 1)

//!
//! Static function prototypes
//! @{
//!
static void timerCallback(void);
static uint32_t pdm_init(void);

//! @}
// End static function prototypes

//
//! @note Apollo3x has 1 PDM
//
void am_pdm0_isr(void); //!< isr function prototype

//*****************************************************************************
//
// PDM interrupt handler.
//
//*****************************************************************************
void
am_pdm0_isr(void)
{

    g_tGlb.ui32PDMIsrCount++ ;


    if ( g_tGlb.g_PeriphHandle == 0 ) {
      while ( 1 ) ;
    }
    am_hal_interupt_service_msg_t ptisrData ;
    am_hal_pdm_interrupt_service(g_tGlb.g_PeriphHandle, &ptisrData, &g_tGlb.tPDMTransfer);


    if (ptisrData.isrStat & AM_HAL_PDM_INT_DCMP)
    {
        g_tGlb.ui32DCMPIsrCount++;
        g_tGlb.bPDMDataReady = true;
        GPIO_DBG1_TGLE;
    }

    if (ptisrData.isrStat & AM_HAL_PDM_INT_OVF)
    {
        g_tGlb.ui32FifoOVFCount++;
    }
    if (ptisrData.isrStat & AM_HAL_PDM_INT_DERR)
    {
        g_tGlb.ui32DMAError++;
    }
}
//*****************************************************************************
//
//! @brief     Timer callback
//!
//! This is called when the timer expires from the timer interrupt
//!
//! @param x   Unused
//
//*****************************************************************************
static void
timerCallback(void)
{
    g_tGlb.bTimerFlag = true;
}

//*****************************************************************************
//
//! @brief init the PDM
//!
//! @return standard hal status
//
//*****************************************************************************
static uint32_t
pdm_init(void)
{
    uint32_t ui32Status;

    g_tDMA.ui32PdmOverflowTest = 0xFFAA81EE;

#if BUFFERTEST_FILL == 1
    {
        for (uint32_t i = 0; i < TESTDATASIZE; i++)
        {
            g_tDMA.ui32TestData[i] = g_patternA + i;
            g_tDMA.ui32TestDataB[i] = g_patternB + i;
            g_tDMA.ui32TestDataC[i] = g_patternC + i;
        }
    }

#endif // BUFFERTEST_FILL == 1

    //
    // Initialize, power-up, and configure the PDM.
    //
    do
    {
        //
        //! Get the handle for this PDM module
        //
        ui32Status = am_hal_pdm_initialize(PDM_MODULE, &g_tGlb.g_PeriphHandle);
        if (ui32Status)
        {
            break;
        }

        ui32Status = am_hal_pdm_power_control(g_tGlb.g_PeriphHandle, AM_HAL_PDM_POWER_ON, false);
        if (ui32Status)
        {
            break;
        }
        ui32Status = am_hal_pdm_configure(g_tGlb.g_PeriphHandle, &g_tPdmConfig);
        if (ui32Status)
        {
            break;
        }

        //
        // enable the clocks to the PDM
        //
        ui32Status = am_hal_pdm_enable(g_tGlb.g_PeriphHandle);
        if (ui32Status)
        {
            break;
        }

        //
        // configure the PDM pins
        //
        ui32Status = pin_setup(gsPinCfg, PN_NUM_PDM_PINS);
        if (ui32Status)
        {
            break;
        }

        ui32Status = am_hal_pdm_fifo_threshold_setup(g_tGlb.g_PeriphHandle, FIFO_THRESHOLD_CNT);
        if (ui32Status)
        {
            break;
        }

        //
        //! init the PDM init struct
        //
        g_tGlb.tPDMTransfer.ui32BufferWithDataAddr = 0;
        g_tGlb.tPDMTransfer.ui32TargetAddr = (uint32_t) g_tDMA.ui32TestData;
        g_tGlb.tPDMTransfer.ui32TargetAddrReverse = (uint32_t) g_tDMA.ui32TestDataB;
        g_tGlb.tPDMTransfer.ui32TotalCount        = TESTDATASIZE * 4;
        g_tGlb.tPDMTransfer.bRestartDMA = false;

        //
        //! setup PDM, do not start
        //
        ui32Status = am_hal_dma_param_setup(g_tGlb.g_PeriphHandle, &g_tGlb.tPDMTransfer);
        if (ui32Status)
        {
            break;
        }

        //
        // Configure and enable PDM interrupts (set up to trigger on DMA
        // completion).
        //
#if USE_DMA
        ui32Status = am_hal_pdm_interrupt_enable(g_tGlb.g_PeriphHandle, (AM_HAL_PDM_INT_DERR
                                                                         | AM_HAL_PDM_INT_DCMP
                                                                         | AM_HAL_PDM_INT_UNDFL
                                                                         | AM_HAL_PDM_INT_OVF));
        if (ui32Status)
        {
            break ;
        }

#else
         ui32Status = am_hal_pdm_interrupt_enable(PDMHandle, (AM_HAL_PDM_INT_DERR
                                                | AM_HAL_PDM_INT_UNDFL
                                                | AM_HAL_PDM_INT_OVF
                                                | AM_HAL_PDM_INT_THR));
#endif

        NVIC_SetPriority(PDM_IRQn, PDM_IRQ_PRIORITY);
        NVIC_ClearPendingIRQ(PDM_IRQn);
        NVIC_EnableIRQ(PDM_IRQn);
    }
    while ( false );

    return ui32Status;
}

//*****************************************************************************
//
//! Main function.
//
//*****************************************************************************
int
main(void)
{
    //
    // Perform the standard initialzation for clocks, cache settings, and
    // board-level low-power operation.
    //
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    uint32_t ui32Stat =AM_HAL_STATUS_SUCCESS;
#if PRINT_ENABLE == 1
    am_bsp_itm_printf_enable();
    am_bsp_debug_printf_enable();
#endif

    am_bsp_low_power_init();

    //
    // Print the banner.
    //
#if PRINT_ENABLE == 1
    //
    // Clear the terminal and print the banner.
    //
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("PDM Power example.\n\n");
#endif

#if GPIO_DEBUG_ENABLE == 1

    ui32Stat = pin_setup(g_tDebugPinCfg, eMAX_DEBUG_PINS);
    if (ui32Stat)
    {
        STARTUP_PRINT("GPIO DEBUG PIN setup failure %d\nExample will Hang\n", ui32Stat);
        while ( 1 );
    }

#endif // GPIO_DEBUG_ENABLE == 1

    //
    // TimerA0 init., 1 second callback,
    //
    ui32Stat = timer_init(TMR_TICKS_PER_SECOND, timerCallback);
    if (ui32Stat)
    {
        STARTUP_PRINT("Timer Init failure %d\nExample will Hang\n", ui32Stat);
        while ( 1 );
    }

#if PDM_MODE >= PDM_INIT_ONLY
    //
    // Turn on the PDM, set it up for the chosen recording settings, and start
    // the first DMA transaction.
    //
    ui32Stat = pdm_init();
    if (ui32Stat)
    {
        STARTUP_PRINT("PDM Init failure %d\nExample will Hang\n", ui32Stat);
        while ( 1 );
    }
#endif

    //
    // Global interrupt enable
    //
    am_hal_interrupt_master_enable();

    if (g_bSequenceMode)
    {
        STARTUP_PRINT("Periodically reading PDM data - %d times\n", NUM_PDM_DMA_XFER_PER_SEQ);
    }
    else
    {
        STARTUP_PRINT("Continuous PDM reading\n");
    }

    //
    // wait for all prints to finish before disabling printing
    //
    am_util_delay_ms(50);

#if PDM_MODE >= PDM_MODE_CONTINUOUS
    //
    // Start continuous PDM data acquisition.
    //
    g_tGlb.tPDMTransfer.bRestartDMA = true;
    GPIO_DBG1_TGLE;

    ui32Stat = am_hal_dma_restart(g_tGlb.g_PeriphHandle);
    if (ui32Stat)
    {
        STARTUP_PRINT("IOM Init failure %d\nExample will Hang\n", ui32Stat);
        while ( 1 );
    }
#endif

#if (PRINT_ENABLE == 1) && (PRINT_IN_LOOP == 0)
    //
    // printing may have been enabled at startup, but not in loop
    //
    am_bsp_debug_printf_disable();
#endif

    while (true)
    {
#if (PDM_MODE >= PDM_MODE_CONTINUOUS)
        //
        // PDM running
        //
        if (g_tGlb.bPDMDataReady)
        {
            //
            // a single PDM DMA has completed and, optionally, restarted
            //
            g_tGlb.bPDMDataReady = false;
            if (g_tGlb.tPDMTransfer.ui32BufferWithDataAddr)
            {
                //
                // DMA transfer complete and data availabe
                //

                g_tGlb.ui32NumDmaXferComplete++;
                //
                // check if in sequence mode and if the sequence is complete
                //
                if (g_bSequenceMode && g_tGlb.bSequenceStarted)
                {
                    if (  ++g_tGlb.ui32numPDMTransfersThisSequence >= NUM_PDM_DMA_XFER_PER_SEQ)
                    {
                        GPIO_DBG2_LO;//  Set debug 2 low sequence is ending, this may happen early
                        //
                        //! end of sequence, stop DMA auto restart (in the ISR)
                        //
                        g_tGlb.tPDMTransfer.bRestartDMA = false;
                        g_tGlb.bSequenceStarted = false;
                        DEBUG_PRINT("\n   sequence complete %lu %lu %lu\n", g_tGlb.ui32NumDmaXferComplete, g_tGlb.ui32PDMIsrCount, g_tGlb.ui32DCMPIsrCount);
                        GPIO_DBG2_LO;//  Set debug 2 low when waiting for timer to start transmission sequence

                    }
                }

                //
                // read the data
                // zero the buffer address, indicating data has been consumed.
                //
                g_tGlb.tPDMTransfer.ui32BufferWithDataAddr = 0;
            }
        }

        if (g_bSequenceMode && g_tGlb.bTimerFlag && !g_tGlb.bSequenceStarted)
        {
            if ( ++g_tGlb.ui32DelayRestartCount >= 2)
            {
                g_tGlb.ui32DelayRestartCount = 0;
                //
                // timer interrupt, in sequence mode, and sequence is not running
                //
                GPIO_DBG2_HI; //  Set debug 2 high when during transmission period
                // do a dma transfer
                if (NUM_PDM_DMA_XFER_PER_SEQ > 0)
                {
                    //
                    // start a new sequence
                    // interrupt service call will restart dma
                    //
                    g_tGlb.tPDMTransfer.bRestartDMA = true;
                }
                //
                // reset loop counter to 1
                //
                g_tGlb.ui32numPDMTransfersThisSequence = 1;
                g_tGlb.bSequenceStarted = true;

                ui32Stat = am_hal_dma_restart(g_tGlb.g_PeriphHandle);

                if (ui32Stat)
                {
                    DEBUG_PRINT("\nPDM io error %lu\n", ui32Stat);
                    break;
                }
                DEBUG_PRINT("\nPDM sequence start %lu %lu %lu\n", g_tGlb.ui32NumDmaXferComplete, g_tGlb.ui32PDMIsrCount, g_tGlb.ui32DCMPIsrCount);
            }

        }

#endif // PDM_MODES

        if (g_tGlb.bTimerFlag)
        {
            //
            // timer interrupt:
            //
            g_tGlb.bTimerFlag = false;

            DEBUG_PRINT("\nPDM second timer Num DMA-Xfer: %lu, PdmIsrCount: %lu, PDM Overflow Count: %lu, PDM DMA Count: %lu, ISR DMA Err: %lu\n", g_tGlb.ui32NumDmaXferComplete, g_tGlb.ui32PDMIsrCount, g_tGlb.ui32FifoOVFCount, g_tGlb.ui32DCMPIsrCount, g_tGlb.ui32DMAError);
        }
        GPIO_SLEEP_LO;
        am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);

        GPIO_SLEEP_HI;
    } // while(1)

    DEBUG_PRINT("\nEnd of PDM PWR Example\n");

    //
    // disable the interrupts in the NVIC.
    //
    timer_disable();
    NVIC_DisableIRQ(PDM_IRQn);
    while (1);
}

//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
