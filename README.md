nrf51-capsense-example
==================

This project includes a driver and example for reading capasitive button sensors from the nRF51x22 device, 
without requiring a dedicated capacitive sensor.
It requires 2 GPIO's per button, and can be used to read up to 8 sensors (using a total of 16 GPIO pins). 

For each capsense channel one GPIO will be configured as an output and one as an input.
An external resistor will have to be connected between the output and input pin (the example was tested using 1MOhm resistors). 
The capacitive sensor will have to be connected between the input pin and ground. 
 
The library in its current state will not work with a SoftDevice enabled, as it accesses the PPI peripheral directly. 

Requirements
------------
- nRF51 SDK version 8.0.0
- nRF51 DK

The project may need modifications to work with later versions or other boards. 

To compile it, clone the repository in the /examples/peripheral/ folder in the nRF51 SDK v8.0.0 or later (or any other folder under /examples/)

About this project
------------------
This application is one of several applications that has been built by the support team at Nordic Semiconductor, as a demo of some particular feature or use case. 
It has not necessarily been thoroughly tested, so there might be unknown issues. It is hence provided as-is, without any warranty. 

However, in the hope that it still may be useful also for others than the ones we initially wrote it for, we've chosen to distribute it here on GitHub. 

The application is built to be used with the official nRF51 SDK. The SDK can be downloaded at https://developer.nordicsemi.com/

Please post any questions about this project on https://devzone.nordicsemi.com.