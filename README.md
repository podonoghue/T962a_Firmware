# T962a Oven Controller Upgrade

This is a replacement controller for a T962a Oven.  
It consists of a complete replacement for the existing controller in a mechnically compatible form.  
It provides open source firmware that provides extended features.

**Features**  
- Supports the two internal thermocouples  
- Support up to two external thermocouples (with expansion board)  
- Supports up to 10 profiles stored in non-voltile memory  
- The current configuration is also stored in non-voltile memory  
- Uses an additional SSD to completely separate the low-voltage and high-voltage components  

**Requirements**  
- It uses a Kinetis microcontroller so requires a compatible programmer e.g. USBDM  
- Firmware development requires Eclipse/KDS and USBDM plugin  

# T962a Firmware

Firmware for T962a replacement oven controller.  
This is a Kinetis Design Studio workspace.   
 
[Firmware is available here](https://github.com/podonoghue/T962a_Oven_Controller/tree/master/Firmware)  
[Driver for USB interface](https://github.com/podonoghue/T962a_Oven_Controller/tree/master/WinDrivers)  

# T962a Hardware (PCB Designs)

PCB designs for T962a replacement oven controller.  
These are Altium designer projects.  
Gerber files are also provided.  
Bare boards may be ordered from OSH Park

[Hardware designs are available here](https://github.com/podonoghue/T962a_Oven_Controller/tree/master/Hardware)

Main board
====

![An image](https://raw.githubusercontent.com/podonoghue/T962a_Oven_Controller/master/Hardware/T962a/T962a.png "Top Board Image")  

Expansion board
====

![An image](https://raw.githubusercontent.com/podonoghue/T962a_Oven_Controller/master/Hardware/T962a_Panel/T962a_Panel.png "Top Board Image")  
