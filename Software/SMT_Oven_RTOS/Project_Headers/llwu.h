/**
 * @file     llwu.h (180.ARM_Peripherals/Project_Headers/llwu.h)
 * @brief    Low leakage wake-up unit
 *
 * @version  V4.12.1.80
 * @date     13 April 2016
 */
#ifndef LLWU_H_
#define LLWU_H_
 /*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include "hardware.h"

namespace USBDM {

/**
 * @addtogroup LLWU_Group LLWU, Low-leakage Wake-up Unit
 * @brief Abstraction for Low-leakage Wake-up Unit
 * @{
 */

/**
 * LLWU pin wake-up mode
 */
enum LlwuPinMode {
   LlwuPinMode_Disabled    = LLWU_PE1_WUPE0(0)|LLWU_PE1_WUPE1(0)|LLWU_PE1_WUPE2(0)|LLWU_PE1_WUPE3(0), //!< Wake-up by pin change disabled
   LlwuPinMode_RisingEdge  = LLWU_PE1_WUPE0(1)|LLWU_PE1_WUPE1(1)|LLWU_PE1_WUPE2(1)|LLWU_PE1_WUPE3(1), //!< Wake-up on pin rising edge
   LlwuPinMode_FallingEdge = LLWU_PE1_WUPE0(2)|LLWU_PE1_WUPE1(2)|LLWU_PE1_WUPE2(2)|LLWU_PE1_WUPE3(2), //!< Wake-up on pin falling edge
   LlwuPinMode_EitherEdge  = LLWU_PE1_WUPE0(3)|LLWU_PE1_WUPE1(3)|LLWU_PE1_WUPE2(3)|LLWU_PE1_WUPE3(3), //!< Wake-up on pin rising or falling edge
};

/**
 * LLWU peripheral wake-up mode
 */
enum LlwuPeripheralMode {
   LlwuPeripheralMode_Disabled = false, //!< Wake-up by peripheral disabled
   LlwuPeripheralMode_Enabled  = true,  //!< Wake-up by peripheral enabled
};

/**
 * LLWU pin sources (MK20 & MK22F)
 */
enum LlwuPin {
   LlwuPin_pte1,   //!< Wake-up pin LLWU_P0
   LlwuPin_pte2,   //!< Wake-up pin LLWU_P1
   LlwuPin_pte4,   //!< Wake-up pin LLWU_P2
   LlwuPin_pta4,   //!< Wake-up pin LLWU_P3
   LlwuPin_pta13,  //!< Wake-up pin LLWU_P4
   LlwuPin_ptb0,   //!< Wake-up pin LLWU_P5
   LlwuPin_ptc1,   //!< Wake-up pin LLWU_P6
   LlwuPin_ptc3,   //!< Wake-up pin LLWU_P7
   LlwuPin_ptc4,   //!< Wake-up pin LLWU_P8
   LlwuPin_ptc5,   //!< Wake-up pin LLWU_P9
   LlwuPin_ptc6,   //!< Wake-up pin LLWU_P10
   LlwuPin_ptc11,  //!< Wake-up pin LLWU_P11
   LlwuPin_ptd0,   //!< Wake-up pin LLWU_P12
   LlwuPin_ptd2,   //!< Wake-up pin LLWU_P13
   LlwuPin_ptd4,   //!< Wake-up pin LLWU_P14
   LlwuPin_ptd6,   //!< Wake-up pin LLWU_P15
};

/**
 * LLWU peripheral sources (MK20 & MK22F)
 */
enum LlwuPeripheral {
   LlwuPeripheral_Lptmr       = (1<<0),  //!< Wake-up peripheral LLWU_M0IF
   LlwuPeripheral_Cmp0        = (1<<1),  //!< Wake-up peripheral LLWU_M1IF
   LlwuPeripheral_Cmp1        = (1<<2),  //!< Wake-up peripheral LLWU_M2IF
   LlwuPeripheral_Reserved3   = (1<<3),  //!< Wake-up peripheral LLWU_M3IF
   LlwuPeripheral_Tsi         = (1<<4),  //!< Wake-up peripheral LLWU_M4IF
   LlwuPeripheral_RtcAlarm    = (1<<5),  //!< Wake-up peripheral LLWU_M5IF
   LlwuPeripheral_Reserved6   = (1<<6),  //!< Wake-up peripheral LLWU_M6IF
   LlwuPeripheral_RtcSeconds  = (1<<7),  //!< Wake-up peripheral LLWU_M7IF
};

/**
 * LLWU pin wake-up mode
 */
enum LlwuFilterPinMode {
   LlwuFilterPinMode_Disabled    = LLWU_FILT_FILTE(0), //!< Wake-up by pin change disabled
   LlwuFilterPinMode_RisingEdge  = LLWU_FILT_FILTE(1), //!< Wake-up on pin rising edge
   LlwuFilterPinMode_FallingEdge = LLWU_FILT_FILTE(2), //!< Wake-up on pin falling edge
   LlwuFilterPinMode_EitherEdge  = LLWU_FILT_FILTE(3), //!< Wake-up on pin rising or falling edge
};

/**
 * Type definition for LLWU interrupt call back
 */
typedef void (*LLWUCallbackFunction)();

template <class Info>
class LlwuBase_T {

protected:
   static constexpr volatile LLWU_Type *llwu = Info::llwu;

public:

   /**
    * Basic enable of LLWU\n
    * Includes configuring all pins
    */
   static void enable() {
      // Configure pins
      Info::initPCRs();
   }

   /**
    * Configure with settings from Configure.usbdmProject.
    */
   static void defaultConfigure() {

      // Configure pins
      Info::initPCRs();

      llwu->PE1   = Info::pe1;
      llwu->PE2   = Info::pe2;
      llwu->PE3   = Info::pe3;
      llwu->PE4   = Info::pe4;

      llwu->ME    = Info::me;

      llwu->FILT1 = Info::filt1|LLWU_FILT_FILTF_MASK;
      llwu->FILT2 = Info::filt2|LLWU_FILT_FILTF_MASK;

      llwu->RST   = Info::rst;

      enableNvicInterrupts();
   }

   /**
    * Configure pin as wake-up source
    *
    * @param[in] llwuPin       Pin to configure
    * @param[in] llwuPinMode   Mode for pin as wake-up input
    */
   static void configurePin(LlwuPin llwuPin, LlwuPinMode llwuPinMode) {
      static const uint8_t masks[] = {(0x3<<0),(0x3<<2),(0x3<<4),(0x3<<6)};
      volatile uint8_t &llwuPe = llwu->PE[llwuPin>>2];
      uint8_t mask = masks[llwuPin&3];
      llwuPe = (llwuPe&~mask) | (llwuPinMode&mask);
   }

   /**
    * Configure peripheral as wake-up source
    *
    * @param[in] llwuPeripheral Peripheral to configure
    * @param[in] llwuPeripheralMode       Whether to enable peripheral as wake-up source
    */
   static void configurePeripheral(LlwuPeripheral llwuPeripheral, LlwuPeripheralMode llwuPeripheralMode) {
      if (llwuPeripheralMode) {
         llwu->ME |= llwuPeripheral;
      }
      else {
         llwu->ME &= (uint8_t)~llwuPeripheral;
      }
   }

   /**
    * Configure one of the input pins as a filtered wake-up source
    *
    * @param[in] filterNum          Filter to configure
    * @param[in] llwuPin            Pin to assign to filter
    * @param[in] llwuFilterPinMode  Mode for pin as wake-up input
    */
   static __attribute__((always_inline)) void configureFilteredInput(int filterNum, LlwuPin llwuPin, LlwuFilterPinMode llwuFilterPinMode) {
      llwu->FILT[filterNum] = llwuPin|llwuFilterPinMode;
   }

   /**
    * Get bit mask indicating wake-up pin sources
    *
    * @return Bit mask
    */
   static __attribute__((always_inline)) uint32_t getPinWakeupSources() {
      return (llwu->PF[1]<<8)|llwu->PF[0];
   }

   /**
    * Clear wake-up pin flag
    *
    *  @param[in] llwuPin Pin indicating which flag to clear
    */
   static __attribute__((always_inline)) void clearWakeupPinFlag(LlwuPin llwuPin) {
      llwu->PF[llwuPin>>3] = (1<<(llwuPin&0x7));
   }

   /**
    * Clear all wake-up pin flags
    */
   static __attribute__((always_inline)) void clearWakeupPinFlags() {
      llwu->PF[0] = 0xFF;
      llwu->PF[1] = 0xFF;
      if (sizeof(llwu->PF) > 2) {
         llwu->PF[2] = 0xFF;
      }
      if (sizeof(llwu->PF) > 3) {
         llwu->PF[3] = 0xFF;
      }
   }

   /**
    * Get flag bit mask indicating wake-up peripheral sources
    *
    * @return Bit mask
    */
   static __attribute__((always_inline)) uint32_t getPeripheralWakeupSources() {
      return llwu->MF;
   }

   /**
    *  Check if pin is source of wake-up
    *
    * @param[in] llwuPin  Pin to check
    *
    * @return false Given pin is not source of wake-up.
    * @return true  Given pin is source of wake-up.
    */
   static __attribute__((always_inline)) bool isPinWakeupSource(LlwuPin llwuPin) {
      return getPinWakeupSources() & (llwuPin&0x7);
   }

   /**
    *  Check if peripheral is source of wake-up
    *
    * @param[in] llwuPeripheral  Peripheral to check
    *
    * @return false Given peripheral is not source of wake-up.
    * @return true  Given peripheral is source of wake-up.
    */
   static __attribute__((always_inline)) bool isPeripheralWakeupSource(LlwuPeripheral llwuPeripheral) {
      return llwu->MF & llwuPeripheral;
   }

   /**
    * Check if filtered wake-up pin is source of wake-up
    *
    * @param[in] filterNum Pin Filter to check
    *
    * @return false Given filtered pin is not source of wake-up.
    * @return true  Given filtered pin is source of wake-up.
    */
   static __attribute__((always_inline)) bool isFilteredInputWakeupSource(int filterNum) {
      return (llwu->FILT[filterNum] & LLWU_FILT_FILTF_MASK);
   }

   /**
    * Enable/disable interrupts in NVIC
    *
    * @param[in]  enable true to enable, false to disable
    */
   static void enableNvicInterrupts(bool enable=true) {

      if (enable) {
         // Enable interrupts
         NVIC_EnableIRQ(Info::irqNums[0]);

         // Set priority level
         NVIC_SetPriority(Info::irqNums[0], Info::irqLevel);
      }
      else {
         // Disable interrupts
         NVIC_DisableIRQ(Info::irqNums[0]);
      }
   }
};

/**
 * Template class to provide LLWU callback
 */
template<class Info>
class LlwuIrq_T : public LlwuBase_T<Info> {

protected:
   /** Callback function for ISR */
   static LLWUCallbackFunction callback;

public:
   /**
    * IRQ handler
    */
   static void irqHandler(void) {
      if (callback != 0) {
         callback();
      }
      else {
         setAndCheckErrorCode(E_NO_HANDLER);
      }
   }

   /**
    * Set Callback function
    *
    *   @param[in]  theCallback - Callback function to be executed on LLWU interrupt
    */
   static void setCallback(LLWUCallbackFunction theCallback) {
      callback = theCallback;
   }
};

template<class Info> LLWUCallbackFunction LlwuIrq_T<Info>::callback = 0;

#ifdef USBDM_LLWU_IS_DEFINED
/**
 * Class representing LLWU
 */
using Llwu = LlwuIrq_T<LlwuInfo>;

#endif

/**
 * End group LLWU_Group
 * @}
 */

} // End namespace USBDM

#endif /* LLWU_H_ */
