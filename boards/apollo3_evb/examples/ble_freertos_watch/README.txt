Name:
=====
 ble_freertos_watch


Description:
============
 ARM Cordio BLE - Concurrent Controller/Target Example.




Purpose:
========
This example demonstrates an BLE application in the Central role.
 That is the BLE application operates as a target to phone controller and as the
 controller of subordinate target devices running ble_freertos_fit example in the
 SDK.


 1. Printing takes place over the ITM at 1M Baud.
 2. When the example powers up,
    2.A. it enters advertising mode by default to wait for connection from
         smartphone with Time profile, Alert Notification profile and Phone
         Alert Status profile supported.
    2.B. when BTN2 on Apollo3 EVB is short-pressed, if advertising is on, it
         stops advertising first and then starts scanning when advertising is
         stopped; if scanning is on, it stops scanning and re-start advertising
         when scanning stops.
    2.C. During scanning, the device (if discovered) running ble_freertos_fit
         example in this SDK will be connected and scanning will be stopped.
    2.D. Repeat 2.B. and 2.C. above to connect to a new target device running
         ble_freertos_fit example (max targets is 3).
 3. When phone (iPhone is used) connects to this example, the services of Time
    profile, Alert Notification profile and Phone Alert Status profile will be
    subscribed by the example.



******************************************************************************


