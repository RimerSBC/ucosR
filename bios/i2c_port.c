/**-----------------------------------------------------------------------------
 * Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0
 * International (CC BY-NC-SA 4.0). 
 * 
 * You are free to:
 *  - Share: Copy and redistribute the material.
 *  - Adapt: Remix, transform, and build upon the material.
 * 
 * Under the following terms:
 *  - Attribution: Give appropriate credit and indicate changes.
 *  - NonCommercial: Do not use for commercial purposes.
 *  - ShareAlike: Distribute under the same license.
 * 
 * DISCLAIMER: This work is provided "as is" without any guarantees. The authors
 * aren’t responsible for any issues, damages, or claims that come up from using
 * it. Use at your own risk!
 * 
 * Full license: http://creativecommons.org/licenses/by-nc-sa/4.0/
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 Sergey Sanders
 * sergey@sesadesign.com
 * ---------------------------------------------------------------------------*/

#include "i2c_port.h"

#include "FreeRTOS.h"
#include "bsp.h"
#include "semphr.h"
#include "task.h"

#define I2C_SW
#define I2C_BUS_TIMEOUT 2000

void i2c_port_init(Sercom* port, i2c_speed_t speed)
{
   const uint8_t speedDiv[] = { 54, 9, 2 }; /// Baud rate selections
   port->I2CM.CTRLA.bit.MODE = 0x05;        /// Set SERCOM to I2C master
   port->I2CM.CTRLA.bit.LOWTOUTEN = 0x01;   /// Enable SCL low timeout
   port->I2CM.CTRLA.bit.SEXTTOEN = 0x01;
   port->I2CM.CTRLA.bit.MEXTTOEN = 0x01;
   port->I2CM.CTRLA.bit.SCLSM = 0x01;              /// Set clock stretching mode to enable only after ACK
   port->I2CM.CTRLA.bit.SPEED = 0x01;              /// Set speed to Fast mode plus (Fm+) up to 1MHz
   port->I2CM.CTRLB.bit.SMEN = 0x01;               /// Enable smart mode
   port->I2CM.BAUD.bit.BAUD = speedDiv[speed];     /// Set baud rate
   port->I2CM.CTRLA.bit.ENABLE = 0x01;             /// Enable SERCOM
   while(port->I2CM.SYNCBUSY.bit.ENABLE);          /// Check to see if SERCOM synchronization is busy
   port->I2CM.STATUS.bit.BUSSTATE = I2C_STAT_IDLE; /// Set bus state to \a I2C_STAT_IDLE
}
// volatile SERCOM_I2CM_STATUS_Type statReg;
// volatile SERCOM_I2CM_CTRLA_Type ctrlAReg;

void i2c_port_reset(Sercom* port)
{
   uint32_t tmpCtrlA = port->I2CM.CTRLA.reg;
   uint32_t tmpCtrlB = port->I2CM.CTRLB.reg;
   uint32_t tmpBaud = port->I2CM.BAUD.reg;
   port->I2CM.CTRLA.bit.SWRST = 0x01;     /// Reset SERCOM
   while(port->I2CM.SYNCBUSY.bit.SWRST);  /// Check to see if SERCOM synchronization is busy
   port->I2CM.BAUD.reg = tmpBaud;         /// Restore sercom state
   port->I2CM.CTRLB.reg = tmpCtrlB;       /// Restore sercom state
   port->I2CM.CTRLA.reg = tmpCtrlA;       /// Restore sercom state
   while(port->I2CM.SYNCBUSY.bit.ENABLE); /// Check to see if SERCOM synchronization is busy
   port->I2CM.STATUS.bit.BUSSTATE = I2C_STAT_IDLE;
}

bool mb_wait(Sercom* port, uint16_t to, i2c_err_t* err)
{
   bool stat = true;
   while(!port->I2CM.INTFLAG.bit.MB)
   {
      if(port->I2CM.STATUS.bit.BUSERR)
      {
         *err = I2C_ERR_BUSERR;
         port->I2CM.STATUS.bit.BUSERR = 1;
         stat = false;
         break;
      }
      else if(!--to)
      {
         *err = I2C_ERR_BUSERR;
         stat = false;
         break;
      }
      taskYIELD();
   }
   return stat;
}

bool sb_wait(Sercom* port, uint16_t to, i2c_err_t* err)
{
   while(!port->I2CM.INTFLAG.bit.SB)
   {
      if(!--to)
      {
         *err = I2C_ERR_BUSERR;
         return false;
      }
      taskYIELD();
   }
   return true;
}
i2c_err_t i2c_write(Sercom* port, uint16_t id, uint16_t addr, uint8_t* buf, uint16_t size)
{
   i2c_sm_t state = I2C_SM_START; /// Set I2C state machine state to \a I2C_SM_START
   i2c_err_t err = I2C_ERR_NONE;
   bool busy = true;
   do {
      switch(state) /// <B> Implementation of I2C write using I2C state machine </B> <HR> <HR>
      {
      case I2C_SM_START:                         /// <B> State machine state is \a I2C_SM_START </B> <HR>
         port->I2CM.ADDR.bit.ADDR = (uint8_t)id; /// Set I2C address to \a i2cAddr. Address is left shifted 1 bit as the
                                                 /// I2C protocol uses bit0 for read/write flag
         state = (id >> 8) ? I2C_SM_ADDR_MSB : I2C_SM_ADDR_LSB; /// Set state machine state to \a I2C_SM_ADD_LSB
         err = I2C_ERR_NODEV;
         break;             /// END \a I2C_SM_START <HR> \n
      case I2C_SM_ADDR_MSB: /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         port->I2CM.DATA.bit.DATA = (uint8_t)(addr >> 8); ///  Send next data byte to SERCOM \n
         state = I2C_SM_ADDR_LSB; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NOACK;
         break;
      case I2C_SM_ADDR_LSB:                                  /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         port->I2CM.CTRLB.bit.ACKACT = (size) ? 0x00 : 0x01; /// Acknowledge action if size of packet left to send is >1
         port->I2CM.DATA.bit.DATA = (uint8_t)addr;           ///  Send next data byte to SERCOM \n
         state = (size) ? I2C_SM_DATA : I2C_SM_DONE; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NOACK;
         break;
      case I2C_SM_DATA: /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         port->I2CM.CTRLB.bit.ACKACT =
            (size >= 1) ? 0x00 : 0x01;                 /// Acknowledge action if size of packet left to send is >1
         port->I2CM.DATA.bit.DATA = *buf++;            ///  Send next data byte to SERCOM \n
         state = (--size) ? I2C_SM_DATA : I2C_SM_DONE; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NOACK;
         break; /// END \a I2C_SM_DATA <HR> \n

      case I2C_SM_DONE: /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         err = I2C_ERR_NONE; /// Exit from state machine with I2C error \a I2C_ERR_NONE
         busy = false;
         break; /// END \a I2C_SM_DONE <HR> \n

      default:                 /// <B> State machine state \a default -> no valid states found </B> <HR>
         err = I2C_ERR_BUSERR; /// Exit from state machine with I2C error \a I2C_ERR_BUSERR
         break;                /// END \a default <HR> \n

      } /// END of I2C write state machine <HR> <HR> \n
      if(!busy) break;
      busy = mb_wait(port, I2C_BUS_TIMEOUT, &err);
      if(port->I2CM.STATUS.bit.ARBLOST || port->I2CM.STATUS.bit.RXNACK) /// Check to see if error occured on bus
      {
         err = (port->I2CM.STATUS.bit.RXNACK)
                  ? err
                  : I2C_ERR_BUSERR; /// Exit from state machine with I2C error \a I2C_ERR_BUSERR or \a I2C_ERR_NOACK
         busy = false;
      }
   } while(busy);
   port->I2CM.CTRLB.bit.CMD = 0x03; /// Issue STOP condition
   while(port->I2CM.SYNCBUSY.bit.SYSOP) asm("nop");
   return err;
}

i2c_err_t i2c_read(Sercom* port, uint16_t id, uint16_t addr, uint8_t* buf, uint16_t size)
{
   i2c_sm_t state = I2C_SM_START; /// Set I2C state machine state to \a I2C_SM_START
   i2c_err_t err = I2C_ERR_NONE;
   bool busy = true;
   bool addrSend = true;
   if(!size)
      return I2C_ERR_PARAM;
   else
      size--; // one byte read in the STOP state
   do {
      switch(state) /// <B> Implementation of I2C read using I2C state machine </B> <HR> <HR>
      {
      case I2C_SM_START:                         /// <B> State machine state is \a I2C_SM_START </B>
         port->I2CM.ADDR.bit.ADDR = (uint8_t)id; /// Set I2C address to \a i2cAddr. Address is left shifted 1 bit as the
                                                 /// I2C protocol uses bit0 for read/write flag
         state = (id >> 8) ? I2C_SM_ADDR_MSB : I2C_SM_ADDR_LSB; /// Set state machine state to \a I2C_SM_ADDRESS
         if((busy = mb_wait(port, I2C_BUS_TIMEOUT, &err)))
            if(port->I2CM.STATUS.bit.ARBLOST || port->I2CM.STATUS.bit.RXNACK) /// Check to see if error occured on bus
            {
               err =
                  (port->I2CM.STATUS.bit.RXNACK)
                     ? I2C_ERR_NODEV
                     : I2C_ERR_BUSERR; /// Exit from state machine with I2C error \a I2C_ERR_BUSERR or \a I2C_ERR_NOACK
               busy = false;
            }
         break;                                           /// END \a I2C_SM_START <HR> \n
      case I2C_SM_ADDR_MSB:                               /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         port->I2CM.DATA.bit.DATA = (uint8_t)(addr >> 8); ///  Send next data byte to SERCOM \n
         state = I2C_SM_ADDR_LSB; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NOACK;
         if((busy = mb_wait(port, I2C_BUS_TIMEOUT, &err)))
            if(port->I2CM.STATUS.bit.ARBLOST ||
               port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
            {
               err = (port->I2CM.STATUS.bit.RXNACK)
                        ? I2C_ERR_NOACK
                        : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged*/
               busy = false;
            }
         break;
      case I2C_SM_ADDR_LSB: /// <B> State machine state is \a I2C_SM_ADDR_LSB </B> <HR>
         if(addrSend)
         {
            addrSend = false;
            port->I2CM.DATA.bit.DATA = (uint8_t)addr;
            state = I2C_SM_ADDR_LSB;
            if((busy = mb_wait(port, I2C_BUS_TIMEOUT, &err)))
               if(port->I2CM.STATUS.bit.ARBLOST ||
                  port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
               {
                  err = (port->I2CM.STATUS.bit.RXNACK)
                           ? I2C_ERR_NOACK
                           : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged*/
                  busy = false;
               }
         }
         else
         {
            port->I2CM.CTRLB.bit.ACKACT =
               (size >= 1) ? 0x00 : 0x01; /// Acknowledge action if size of packet left to send is >1
            port->I2CM.ADDR.bit.ADDR = ((uint8_t)id) | 0x01; /// Prepare to read data
            if(!size)
            {
               port->I2CM.CTRLB.bit.ACKACT = 0x01; /// Set action acknowledged
               state = I2C_SM_DONE;                /// Set state machine state to \a I2C_DONE
            }
            else
               state = I2C_SM_DATA; /// If there is no data left to send change state machine state to \a IC2_SM_DONE,
                                    /// if data left leave state as \a I2C_SM_DATA
            if((busy = sb_wait(port, I2C_BUS_TIMEOUT, &err)))
               if(port->I2CM.STATUS.bit.ARBLOST ||
                  port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
               {
                  err = (port->I2CM.STATUS.bit.RXNACK)
                           ? I2C_ERR_NOACK
                           : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged*/
                  busy = false;
               }
         }
         break; /// END \a I2C_SM_ADDRESS <HR> \n

      case I2C_SM_DATA:                     /// <B> State machine state is \a I2C_SM_DATA </B> <HR>
         *buf++ = port->I2CM.DATA.bit.DATA; /// Store packet is memory pointed to by \a data
         if(!--size)                        /// If all data has been read
         {
            port->I2CM.CTRLB.bit.ACKACT = 0x01; /// Set action acknowledged
            state = I2C_SM_DONE;                /// Set state machine state to \a I2C_DONE
         }
         if((busy = sb_wait(port, I2C_BUS_TIMEOUT, &err)))
            if(port->I2CM.STATUS.bit.ARBLOST ||
               port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
            {
               err = (port->I2CM.STATUS.bit.RXNACK)
                        ? I2C_ERR_NOACK
                        : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged
               busy = false;
            }
         break; /// END \a I2C_SM_DATA <HR> \n

      case I2C_SM_DONE:                        /// <B> State machine is \a I2C_SM_DONE </B> <HR>
         *buf = port->I2CM.DATA.bit.DATA;      /// Store packet in \a data
         err = I2C_ERR_NONE;                   /// Return no error
         busy = false;
         break; /// END \a I2C_SM_DONE <HR> \n

      default:                 /// <B> State machine state \a default -> no valid states found </B> <HR>
         err = I2C_ERR_BUSERR; /// State machine default case - return bus error
         busy = false;
         break; /// END \a default <HR> \n

      } /// END of I2C write state machine <HR> <HR> \n
   } while(busy);
      port->I2CM.CTRLB.bit.CMD = 0x03; /// Issue STOP condition
      while(port->I2CM.SYNCBUSY.bit.SYSOP) asm("nop"); /// While synchronizing is being completed
   return err;
}

i2c_err_t i2c_bare_write(Sercom* port, uint8_t id, uint8_t* buf, uint16_t size)
{
   i2c_sm_t state = I2C_SM_START; /// Set I2C state machine state to \a I2C_SM_START
   i2c_err_t err = I2C_ERR_NONE;
   bool busy = true;
   do {
      switch(state) /// <B> Implementation of I2C write using I2C state machine </B> <HR> <HR>
      {
      case I2C_SM_START:                /// <B> State machine state is \a I2C_SM_START </B> <HR>
         port->I2CM.ADDR.bit.ADDR = id; /// Set I2C address to \a i2cAddr. Address is left shifted 1 bit as the I2C
                                        /// protocol uses bit0 for read/write flag
         state = (size) ? I2C_SM_DATA : I2C_SM_DONE; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NODEV;
         break; /// END \a I2C_SM_START <HR> \n

      case I2C_SM_DATA: /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         port->I2CM.CTRLB.bit.ACKACT =
            (size >= 1) ? 0x00 : 0x01;                 /// Acknowledge action if size of packet left to send is >1
         port->I2CM.DATA.bit.DATA = *buf++;            ///  Send next data byte to SERCOM \n
         state = (--size) ? I2C_SM_DATA : I2C_SM_DONE; /// If last byte send set state machine state to \a I2C_SM_DONE
         err = I2C_ERR_NOACK;
         break; /// END \a I2C_SM_DATA <HR> \n

      case I2C_SM_DONE:                        /// <B> State machine state is \a I2C_SM_DONE </B> <HR>
         err = I2C_ERR_NONE;                   /// Exit from state machine with I2C error \a I2C_ERR_NONE
         busy = false;
         break; /// END \a I2C_SM_DONE <HR> \n

      default:                 /// <B> State machine state \a default -> no valid states found </B> <HR>
         err = I2C_ERR_BUSERR; /// Exit from state machine with I2C error \a I2C_ERR_BUSERR
         break;                /// END \a default <HR> \n

      } /// END of I2C write state machine <HR> <HR> \n
      if(!busy) break;
      busy = mb_wait(port, I2C_BUS_TIMEOUT, &err);
      if(port->I2CM.STATUS.bit.ARBLOST || port->I2CM.STATUS.bit.RXNACK) /// Check to see if error occured on bus
      {
         err = (port->I2CM.STATUS.bit.RXNACK)
                  ? err
                  : I2C_ERR_BUSERR; /// Exit from state machine with I2C error \a I2C_ERR_BUSERR or \a I2C_ERR_NOACK
         busy = false;
      }
   } while(busy);
      port->I2CM.CTRLB.bit.CMD = 0x03; /// Issue STOP condition
      while(port->I2CM.SYNCBUSY.bit.SYSOP) asm("nop"); /// While synchronizing is being completed
   return err;
}

i2c_err_t i2c_bare_read(Sercom* port, uint8_t id, uint8_t* buf, uint16_t size)
{
   i2c_sm_t state = I2C_SM_START; /// Set I2C state machine state to \a I2C_SM_START
   i2c_err_t err = I2C_ERR_NONE;
   bool busy = true;
   if(!size)
      return I2C_ERR_PARAM;
   else
      size--; // one byte read in the STOP state
   do {
      switch(state) /// <B> Implementation of I2C read using I2C state machine </B> <HR> <HR>
      {
      case I2C_SM_START: /// <B> State machine state is \a I2C_SM_START </B>
         port->I2CM.CTRLB.bit.ACKACT =
            (size >= 1) ? 0x00 : 0x01;                    /// Acknowledge action if size of packet left to send is >1
         port->I2CM.ADDR.bit.ADDR = ((uint8_t)id) | 0x01; /// Prepare to read data
         if(!size)
         {
            port->I2CM.CTRLB.bit.ACKACT = 0x01; /// Set action acknowledged
            state = I2C_SM_DONE;                /// Set state machine state to \a I2C_DONE
         }
         else
            state = I2C_SM_DATA; /// If there is no data left to send change state machine state to \a IC2_SM_DONE, if
                                 /// data left leave state as \a I2C_SM_DATA
         if((busy = sb_wait(port, I2C_BUS_TIMEOUT, &err)))
            if(port->I2CM.STATUS.bit.ARBLOST ||
               port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
            {
               err = (port->I2CM.STATUS.bit.RXNACK)
                        ? I2C_ERR_NOACK
                        : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged*/
               busy = false;
            }
         break; /// END \a I2C_SM_ADDRESS <HR> \n

      case I2C_SM_DATA:                     /// <B> State machine state is \a I2C_SM_DATA </B> <HR>
         *buf++ = port->I2CM.DATA.bit.DATA; /// Store packet is memory pointed to by \a data
         if(!--size)                        /// If all data has been read
         {
            port->I2CM.CTRLB.bit.ACKACT = 0x01; /// Set action acknowledged
            state = I2C_SM_DONE;                /// Set state machine state to \a I2C_DONE
         }
         if((busy = sb_wait(port, I2C_BUS_TIMEOUT, &err)))
            if(port->I2CM.STATUS.bit.ARBLOST ||
               port->I2CM.STATUS.bit.RXNACK) /// Check for arbitration lost and receive not acknowledge
            {
               err = (port->I2CM.STATUS.bit.RXNACK)
                        ? I2C_ERR_NOACK
                        : I2C_ERR_BUSERR; /// Return error if arbitration is lost or receive not acknowledged
               busy = false;
            }
         break; /// END \a I2C_SM_DATA <HR> \n

      case I2C_SM_DONE:                        /// <B> State machine is \a I2C_SM_DONE </B> <HR>
         *buf = port->I2CM.DATA.bit.DATA;      /// Store packet in \a data
         err = I2C_ERR_NONE;                   /// Return no error
         busy = false;
         break; /// END \a I2C_SM_DONE <HR> \n

      default:                 /// <B> State machine state \a default -> no valid states found </B> <HR>
         err = I2C_ERR_BUSERR; /// State machine default case - return bus error
         busy = false;
         break; /// END \a default <HR> \n

      } /// END of I2C write state machine <HR> <HR> \n
   } while(busy);
      port->I2CM.CTRLB.bit.CMD = 0x03; /// Issue STOP condition
      while(port->I2CM.SYNCBUSY.bit.SYSOP) asm("nop"); /// While synchronizing is being completed
   return err;
}
