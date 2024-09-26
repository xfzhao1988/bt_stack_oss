Name:
=====
 ble_freertos_amota_dtm


Description:
============
 ARM Cordio BLE - Ambiq Micro Over the Air (AMOTA) Example with DTM




Purpose:
========
This example implements Ambiq Micro Over-the-Air (OTA) slave. This
 examples also allows Direct Test Mode to set TX to constant trans mode for
 SRRC/FCC/CE. This example is designed to allow loading of a binary
 software update from either and iOS or Android phone running Ambiq's
 application. This example works with the Apollo3 Secure Bootloader (SBL) to
 place the image in flash and then reset the Apollo3 to allow SBL to
 validate and install the image.

 To Enter Direct Test Mode, the user will press BTN0 (PIN16) on the EVB.

 AM_DEBUG_PRINTF
 WSF_TRACE_ENABLED=1
 DTM_MODE=1

 If enabled, debug messages will be sent over ITM at 1M Baud.

Additional Information:
=======================
 The directory \tools\apollo3_amota\scripts contains a Makefile which will
 build the OTA binary.

 The directory \docs\app_notes\amota explains how to use the Ambiq iOS and
 Android applications.


******************************************************************************


