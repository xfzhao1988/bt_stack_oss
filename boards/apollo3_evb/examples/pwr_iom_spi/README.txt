Name:
=====
 pwr_iom_spi


Description:
============
 Example for IOM SPI DMA power measurements.




Purpose:
========
Simple example that uses HAL to queue multiple SPI DMA transfers.


    1. Enter deepsleep during DMA transfer.
    2. DMA write and read-loop every 1s.
    3. Have configurable SPI buffer size.
    4. GPIO indicator for CPU wake state.
    5. Run for a configurable number of loops.


 Pin connections
 SPI:
     This varies depending on IOM selected
     pins used for default IOM 0:
         GPIO[5]  IOM0 SPI SCK
         GPIO[7]  IOM0 SPI MOSI
         GPIO[6]  IOM0 SPI MISO
         GPIO[50] IOM0 SPI nCE0

 Debug Pins Used
     ReadLow_WriteHigh 0
     Sequence          3
     Sleep             2





******************************************************************************


