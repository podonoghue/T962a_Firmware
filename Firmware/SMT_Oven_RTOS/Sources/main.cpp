/*
 ============================================================================
 * @file    main.cpp (180.ARM_Peripherals)
 * @brief   Basic C++ demo using GPIO class
 *
 *  Created on: 10/1/2016
 *      Author: podonoghue
 ============================================================================
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cmsis.h"
#include "system.h"
#include "derivative.h"
#include "hardware.h"
#include "delay.h"
#include "spi.h"
#include "fonts.h"
#include "configure.h"
#include "pid.h"
#include "settings.h"
#include "messageBox.h"
#include "editProfile.h"
#include "mainMenu.h"
#include "usb.h"
#include "utilities.h"
#include "CDCInterface.h"

class profilesMenu {

public:
   static void run() {
      unsigned profileIndex = ::profileIndex;
      bool needUpdate = true;

      for(;;) {
         if (needUpdate) {
            RunProfile::drawProfile(profiles[profileIndex]);
            lcd.refreshImage();
            lcd.setGraphicMode();
            needUpdate = false;
         }
         switch(buttons.getButton()) {
         case SW_F1:
            if (profileIndex>0) {
               profileIndex--;
               needUpdate = true;
            }
            break;
         case SW_F2:
            if ((profileIndex+1)<(sizeof(profiles)/sizeof(profiles[0]))) {
               profileIndex++;
               needUpdate = true;
            }
            break;
         case SW_F3:
            EditProfile::run(profiles[profileIndex]);
            needUpdate = true;
            break;
         case SW_S:
            ::profileIndex.operator =(profileIndex);
            return;
         default:
            break;
         }
         __WFI();
      };
   }
};

void initialise() {
   CDC_Interface::initialise();

   lcd.initialise();
   caseTemperatureMonitor.initialise();
   pid.initialise();
   buttons.initialise();
   Buzzer::init();
   OvenFanLed::setOutput();
   OvenFanLed::low();
   HeaterLed::setOutput();
   HeaterLed::low();
   Spare::enable();
   Spare::setDutyCycle(0);
}

int main() {
   PRINTF("Starting\n");
   initialise();

   USBDM::mapAllPins();

   initialise();
   if (USBDM::getError() != USBDM::E_NO_ERROR) {
      char buff[100];
      lcd.clear();
      lcd.printf("Error in initialisation \n  %s\n", USBDM::getErrorMessage());
      lcd.putString(buff);
      PRINTF("Error in initialisation \n  %s\n", USBDM::getErrorMessage());
   }

   USBDM::Usb0::initialise();

   lcd.clear();

   for (;;) {
      MainMenu::run();
      __WFI();
   }
   return 0;
}
