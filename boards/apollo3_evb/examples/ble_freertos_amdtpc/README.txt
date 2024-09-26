Name:
=====
 ble_freertos_amdtpc


Description:
============
 ARM Cordio BLE - AMDTP Client (Master) Example.




Purpose:
========
This example is the client (master) for the BLE Ambiq Micro Data
 Transfer Protocol.  This example is meant to run on an Apollo3 EVB along
 with another Apollo3 EVB acting as the server. This example provides a
 UART command line interface with a simple menu that allows the user to
 scan, connect, and initiate data transfers from either M->S or S->M
 direction.
 
 The menu will print over the UART when the example is up and running.

 Select BLE_MENU_ID_GAP to start the process:
   --------------------BLE main menu--------------------
   1. BLE_MENU_ID_GAP
   2. BLE_MENU_ID_GATT
   3. BLE_MENU_ID_AMDTP
 
   hint: use 'h' to do main menu
   -----------------------------------------------------
 
 The next Menu allows you to search for the server unit.
   1. Start Scan
   2. Stop Scan
   3. Show Scan Results
   4. Create Connection

 Select #1 Start Scan. This will start a BLE device search.
 Displayed will be "scan start" followed by "scan stop".

 Once the search is complete, the devices list will contain the server.

 Next, select #3 Show Scan Results. This will then display the devices found.
   --------------------Scan Results--------------------
   0 : 0 486c1d68e890
   1 : 0 e8a271556670
   2 : 1 0c0deb165e4c
   3 : 1 8c60323038c5
   4 : 1 8b13f7911e4e
   5 : 1 391c353830c3
   -----------------------------------------------------

 Next, select #4 Create Connection. This will then display the devices found.
   choose an idx from scan results to connect:
   --------------------Scan Results--------------------
   0 : 0 486c1d68e890
   1 : 0 e8a271556670
   2 : 1 0c0deb165e4c
   3 : 1 8c60323038c5
   4 : 1 8b13f7911e4e
   5 : 1 391c353830c3
   -----------------------------------------------------

 Once the connection is established, you may begin the Data Transfer Testing.

 Printing takes place over the ITM at 1M Baud.



******************************************************************************


