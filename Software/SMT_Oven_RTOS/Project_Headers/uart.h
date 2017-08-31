/****************************************************************************************************//**
 * @file     uart.h (180.ARM_Peripherals/Project_Headers/uart.h)
 * @brief    Universal Asynchronous Receiver/Transmitter interface
 *
 * @version  V0.0
 * @date     2015/06
 *
 *******************************************************************************************************/

#ifndef INCLUDE_USBDM_UART_H_
#define INCLUDE_USBDM_UART_H_
/*
 * *****************************
 * *** DO NOT EDIT THIS FILE ***
 * *****************************
 *
 * This file is generated automatically.
 * Any manual changes will be lost.
 */
#include <stdint.h>
#include "derivative.h"
#include "hardware.h"
#include "formatted_io.h"
#include "queue.h"

namespace USBDM {

/**
 * @addtogroup UART_Group UART, Universal Asynchronous Receiver/Transmitter
 * @brief C++ Class allowing access to UART interface
 * @{
 */

/**
 * Enumeration selecting interrupt sources
 */
enum UartInterrupt {
   UartInterrupt_TxHoldingEmpty  = UART_C2_TIE(1),   //!< Interrupt request on Transmit holding register empty
   UartInterrupt_TxComplete      = UART_C2_TCIE(1),  //!< Interrupt request on Transmit complete
   UartInterrupt_RxFull          = UART_C2_RIE(1),   //!< Interrupt request on Receive holding full
   UartInterrupt_IdleDetect      = UART_C2_ILIE(1),  //!< Interrupt request on Idle detection
};

/**
 * Enumeration selecting direct memory access sources
 */
enum UartDma {
#ifdef UART_C5_TDMAS
   UartDma_TxHoldingEmpty  = UART_C5_TDMAS(1),   //!< DMA request on Transmit holding register empty
   UartDma_RxFull          = UART_C5_RDMAS(1),   //!< DMA request on Receive holding full
#endif
#ifdef UART_C5_TDMAE
   UartDma_TxHoldingEmpty  = UART_C5_TDMAE(1),   //!< DMA request on Transmit holding register empty
   UartDma_RxFull          = UART_C5_RDMAE(1),   //!< DMA request on Receive holding full
#endif
};

/**
 * @brief Base class for UART interface
 */
class Uart : public FormattedIO {

protected:
   /**
    * Check if character is available
    *
    * @return true  Character available i.e. _readChar() will not block
    * @return false No character available
    */
   virtual bool _isCharAvailable() override {
      return (uart->S1 & UART_S1_RDRF_MASK);
   }

   /**
    * Receives a single character (blocking)
    *
    * @return Character received
    */
   virtual int _readChar() override {

      // Get status from UART
      uint8_t status;
      do {
         // Get status from UART
         status = uart->S1;
         // Clear & ignore pending errors
         if ((status & (UART_S1_FE_MASK|UART_S1_OR_MASK|UART_S1_PF_MASK|UART_S1_NF_MASK)) != 0) {
            clearError();
         }
         // Check for Rx buffer full
      } while ((status & UART_S1_RDRF_MASK) == 0);
      return (uint8_t)(uart->D);
   }

   /**
    * Writes a character (blocking)
    *
    * @param[in]  ch - character to send
    */
   virtual void _writeCh(char ch) override {
      while ((uart->S1 & UART_S1_TDRE_MASK) == 0) {
         // Wait for Tx buffer empty
         __asm__("nop");
      }
      uart->D = ch;
      if (ch=='\n') {
         write('\r');
      }
   }

   /**
    * Handler for interrupts when no handler set
    */
   static void unexpectedInterrupt(uint8_t) {
      setAndCheckErrorCode(E_NO_HANDLER);
   }

public:

   /**
    * UART hardware instance
    */
   volatile UART_Type * const uart;

   /**
    * Construct UART interface
    *
    * @param[in]  uart Base address of UART hardware
    */
   Uart(volatile UART_Type *uart) : uart(uart) {
   }

   /**
    * Set baud factor value for interface
    *
    * This is calculated from baud rate and UART clock frequency
    *
    * @param[in]  baudrate       Interface speed in bits-per-second
    * @param[in]  clockFrequency Frequency of UART clock
    */
   void __attribute__((noinline)) setBaudRate(uint32_t baudrate, uint32_t clockFrequency) {

      // Disable UART before changing registers
      uint8_t c2Value = uart->C2;
      uart->C2 = 0;

      // Calculate UART clock setting (5-bit fraction at right)
      int scaledBaudValue = (2*clockFrequency)/baudrate;

#ifdef UART_C4_BRFA_MASK
      // Set Baud rate register
      uart->BDH = (uart->BDH&~UART_BDH_SBR_MASK) | UART_BDH_SBR((scaledBaudValue>>(8+5)));
      uart->BDL = UART_BDL_SBR(scaledBaudValue>>5);
      // Fractional divider to get closer to the baud rate
      uart->C4 = (uart->C4&~UART_C4_BRFA_MASK) | UART_C4_BRFA(scaledBaudValue);
#else
      scaledBaudValue += 1<<4; // Round value
      // Set Baud rate register
      uart->BDH = (uart->BDH&~UART_BDH_SBR_MASK) | UART_BDH_SBR((scaledBaudValue>>(8+5)));
      uart->BDL = UART_BDL_SBR(scaledBaudValue>>5);
#endif

      // Restore UART settings
      uart->C2 = c2Value;
   }

   /**
    * Set baud factor value for interface
    *
    * This is calculated from baud rate and LPUART clock frequency
    *
    * @param[in]  baudrate  Interface speed in bits-per-second
    */
   virtual void setBaudRate(unsigned baudrate) = 0;

   /**
    * Clear UART error status
    */
   virtual void clearError() = 0;

   /**
    * Enable/disable an interrupt source
    *
    * @param[in] uartInterrupt Interrupt source to modify
    * @param[in] enable        True to enable, false to disable
    *
    * @note Changing the enabled interrupt functions may also affect the DMA settings
    */
   void enableInterrupt(UartInterrupt uartInterrupt, bool enable=true) {
      if (enable) {
#ifdef UART_C5_TDMAS
         uart->C5 &= ~uartInterrupt; // DMA must be off to enable interrupts
#endif
         uart->C2 |= uartInterrupt;
      }
      else {
         uart->C2 &= ~uartInterrupt; // May also disable DMA
      }
   }

   /**
    * Enable/disable a DMA source
    *
    * @param[in] uartDma  Interrupt source to modify
    * @param[in] enable   True to enable, false to disable
    *
    * @note Changing the enabled DMA functions may also affect the interrupt settings
    */
   void enableDma(UartDma uartDma, bool enable=true) {
      // Flags are in same positions in the C2 and C5
      if (enable) {
         uart->C5 |= uartDma;
#ifdef UART_C5_TDMAS
         uart->C2 |= uartDma; // Interrupts must be enable for DMA
#endif
      }
      else {
#ifdef UART_C5_TDMAS
         uart->C2 &= ~uartDma; // Switching DMA off shouldn't enable interrupts!
#endif
         uart->C5 &= ~uartDma;
      }
   }

   /**
    *  Flush output data
    */
   virtual void flushOutput() override {
      while ((uart->S1 & UART_S1_TC_MASK) == 0) {
      // Wait until transmission of last character is complete
      }
   };

   /**
    *  Flush input data
    */
   virtual void flushInput() override {
      (void)uart->D;
      lookAhead = -1;
   };
};

/**
 * Type definition for UART interrupt call back
 *
 *  @param[in]  status - Interrupt flags e.g. UART_S1_TDRE, UART_S1_RDRF etc
 */
typedef void (*UARTCallbackFunction)(uint8_t status);

/**
 * @brief Template class representing an UART interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  Uart *uart0 = new USBDM::Uart_T<Uart0Info>(115200);
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 *
 * @tparam Info   Class describing UART hardware
 */
template<class Info> class Uart_T : public Uart {

protected:
   /** Callback function for ISR */
   static UARTCallbackFunction rxTxCallback;
   static UARTCallbackFunction errorCallback;

public:
   /**
    * Construct UART interface
    *
    * @param[in]  baudrate         Interface speed in bits-per-second
    */
   Uart_T(unsigned baudrate=Info::defaultBaudRate) : Uart(Info::uart) {
      // Enable clock to UART interface
      *Info::clockReg |= Info::clockMask;

      // Configure pins
      Info::initPCRs();
      setBaudRate(baudrate);

      uart->C2 = UART_C2_TE(1)|UART_C2_RE(1);
   }

   /**
    * Destructor
    */
   ~Uart_T() {}

   /**
    * Set baud factor value for interface
    *
    * This is calculated from baud rate and LPUART clock frequency
    *
    * @param[in]  baudrate Interface speed in bits-per-second
    */
   void INLINE_RELEASE setBaudRate(unsigned baudrate) {
      Uart::setBaudRate(baudrate, Info::getInputClockFrequency());
   }

protected:
   /**
    * Clear UART error status
    */
   virtual void clearError() {
      if (Info::statusNeedsWrite) {
         uart->S1 = 0xFF;
      }
      else {
         (void)uart->D;
      }
   }

public:
   /**
    * Receive/Transmit IRQ handler
    */
   static void irqRxTxHandler() {
      uint8_t status = Info::uart->S1;
      rxTxCallback(status);
   }

   /**
    * Error and LON event IRQ handler
    */
   static void irqErrorHandler() {
      uint8_t status = Info::uart->S1;
      errorCallback(status);
   }

   /**
    * Set Receive/Transmit Callback function
    *
    *   @param[in]  callback Callback function to be executed on UART receive or transmit
    */
   static void setRxTxCallback(UARTCallbackFunction callback) {
      if (callback == nullptr) {
         rxTxCallback = unexpectedInterrupt;
      }
      rxTxCallback = callback;
   }

   /**
    * Set Error Callback function
    *
    *   @param[in]  callback Callback function to be executed on UART receive or transmit
    */
   static void setErrorCallback(UARTCallbackFunction callback) {
      if (callback == nullptr) {
         errorCallback = unexpectedInterrupt;
      }
      errorCallback = callback;
   }

   /**
    * Enable/disable interrupts in NVIC
    *
    * @param[in] enable true to enable, false to disable
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

template<class Info> UARTCallbackFunction Uart_T<Info>::rxTxCallback  = unexpectedInterrupt;
template<class Info> UARTCallbackFunction Uart_T<Info>::errorCallback = unexpectedInterrupt;

/**
 * @brief Template class representing an UART interface with buffered reception
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  Uart *uart0 = new USBDM::Uart_T<Uart0Info>(115200);
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 *
 * @tparam Info   Class describing UART hardware
 */
template<class Info, int rxSize=Info::receiveBufferSize, int txSize=Info::transmitBufferSize>
class UartBuffered_T : public Uart_T<Info> {

public:
   using Uart_T<Info>::uart;

   UartBuffered_T(unsigned baudrate=Info::defaultBaudRate) : Uart_T<Info>(baudrate) {
      Uart::enableInterrupt(UartInterrupt_RxFull);
      Uart_T<Info>::enableNvicInterrupts();
   }

   virtual ~UartBuffered_T() {
      Uart::enableInterrupt(UartInterrupt_RxFull,         false);
      Uart::enableInterrupt(UartInterrupt_TxHoldingEmpty, false);
   }

protected:
   /**
    * Queue for Buffered reception (if used)
    */
   static Queue<char, rxSize> rxQueue;
   /**
    * Queue for Buffered transmission (if used)
    */
   static Queue<char, txSize> txQueue;

   /**
    * Writes a character (blocking on queue full)
    *
    * @param[in]  ch - character to send
    */
   virtual void _writeCh(char ch) override {
      // Wait for space in buffer
      while (txQueue.isFull()) {
         __asm__("nop");
      }
      txQueue.enQueue(ch);
      uart->C2 |= UART_C2_TE_MASK;
      if (ch=='\n') {
        _writeCh('\r');
      }
   }

   /**
    * Receives a single character (blocking)
    *
    * @return Character received
    */
   virtual int _readChar() override {
      while (rxQueue.isEmpty()) {
         __asm__("nop");
      }
      return rxQueue.deQueue();
   }

   /**
    * Check if character is available
    *
    * @return true  Character available i.e. _readChar() will not block
    * @return false No character available
    */
   virtual bool _isCharAvailable() override {
      return (!rxQueue.isEmpty());
   }

public:
   /**
    * Receive/Transmit IRQ handler
    */
   static void irqRxTxHandler()  {
      uint8_t status = Info::uart->S1;
      if (status & UART_S1_RDRF_MASK) {
         // Receive data register full - save data
         rxQueue.enQueueDiscardOnFull(Info::uart->D);
      }
      if (status & UART_S1_TDRE_MASK) {
         // Transmitter ready
         if (txQueue.isEmpty()) {
            // No data available - disable further transmit interrupts
            Info::uart->C2 &= ~UART_C2_TE_MASK;
         }
         else {
            // Transmit next byte
            Info::uart->D = txQueue.deQueue();
         }
      }
   }

   /**
    * Error and LON event IRQ handler
    */
   static void irqErrorHandler() {
      // Ignore errors
      clearError();
   }

   /**
    *  Flush output data.
    *  This blocks until all pending data has been sent
    */
   virtual void flushOutput() override {
      while (!txQueue.isEmpty()) {
         // Wait until queue empty
      }
      while ((uart->S1 & UART_S1_TC_MASK) == 0) {
         // Wait until transmission of last character is complete
      }
   }

   /**
    *  Flush input data
    */
   virtual void flushInput() override {
      Uart_T<Info>::flushInput();
      rxQueue.clear();
   }

};

template<class Info, int rxSize, int txSize> Queue<char, rxSize> UartBuffered_T<Info, rxSize, txSize>::rxQueue;
template<class Info, int rxSize, int txSize> Queue<char, txSize> UartBuffered_T<Info, rxSize, txSize>::txQueue;

#ifdef USBDM_UART0_IS_DEFINED
/**
 * @brief Class representing UART0 interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  USBDM::Uart0 uart;
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 */
typedef  Uart_T<Uart0Info> Uart0;
#endif

#ifdef USBDM_UART1_IS_DEFINED
/**
 * @brief Class representing UART1 interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  USBDM::Uart1 uart;
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 */
typedef  Uart_T<Uart1Info> Uart1;
#endif

#ifdef USBDM_UART2_IS_DEFINED
/**
 * @brief Class representing UART2 interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  USBDM::Uart2 uart;
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 */
typedef  Uart_T<Uart2Info> Uart2;
#endif

#ifdef USBDM_UART3_IS_DEFINED
/**
 * @brief Class representing UART3 interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  USBDM::Uart3 uart;
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 */
typedef  Uart_T<Uart3Info> Uart3;
#endif

#ifdef USBDM_UART4_IS_DEFINED
/**
 * @brief Class representing UART4 interface
 *
 * <b>Example</b>
 * @code
 *  // Instantiate interface
 *  USBDM::Uart4 uart;
 *
 *  for(int i=0; i++;) {
 *     uart<<"Hello world, i="<<i<<"\n";
 *  }
 *  @endcode
 */
typedef  Uart_T<Uart4Info> Uart4;
#endif

/**
 * @}
 */

} // End namespace USBDM

#endif /* INCLUDE_USBDM_UART_H_ */
