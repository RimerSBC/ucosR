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
#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include "sam.h"

#include "stdint.h"
#include "stdbool.h"

/// GPIO init

#define PWR_UART_PORT       PORT->Group[0] // PortA
#define PWR_PORT            PORT->Group[1] // PortB
#define PWR_UART_EN         PORT_PA07
#define PWR_DIO0_EN         PORT_PB00
#define PWR_DIO1_EN         PORT_PB01
#define PWR_DIO0_OC         PORT_PB05
#define PWR_DIO1_OC         PORT_PB31
#define PWR_PIN_VDDn        PORT_PB07   // VDD enable, active low

/// PIO ports init
/// Digital
#define DIO0_PORT            PORT->Group[0] // PortA
#define DIO_PORT             PORT->Group[0] // PortA
#define DIO0_PIN_PAD0        PORT_PA16         
#define DIO0_PIN_PAD1        PORT_PA17         
#define DIO0_PIN_PAD2        PORT_PA18         
#define DIO0_PIN_PAD3        PORT_PA19         
#define DIO0_PIN_WO0         PORT_PA20         
#define DIO0_PIN_WO1         PORT_PA21
#define DIO0_PIN_MASK        (DIO0_PIN_PAD0|DIO0_PIN_PAD1|DIO0_PIN_PAD2|DIO0_PIN_PAD3|DIO0_PIN_WO0|DIO0_PIN_WO1)          
#define DIO0_SERCOM          SERCOM1         
#define DIO0_TIMER           TCC0
#define SIO0_PINS_SPI        (DIO0_PIN_PAD0|DIO0_PIN_PAD1|DIO0_PIN_PAD2|DIO0_PIN_PAD3)
#define SIO0_PINS_TRX        (DIO1_PIN_PAD0|DIO0_PIN_PAD1)

#define DIO1_PORT            PORT->Group[0] // PortA
#define DIO1_PIN_PAD0        PORT_PA12        
#define DIO1_PIN_PAD1        PORT_PA13         
#define DIO1_PIN_PAD2        PORT_PA14         
#define DIO1_PIN_PAD3        PORT_PA15         
#define DIO1_PIN_WO0         PORT_PA00         
#define DIO1_PIN_WO1         PORT_PA01         
#define DIO1_PIN_MASK        (DIO1_PIN_PAD0|DIO1_PIN_PAD1|DIO1_PIN_PAD2|DIO1_PIN_PAD3|DIO1_PIN_WO0|DIO1_PIN_WO1)         
#define DIO1_SERCOM          SERCOM2
#define DIO1_TIMER           TC2
#define SIO1_PINS_SPI        (DIO1_PIN_PAD0|DIO1_PIN_PAD1|DIO1_PIN_PAD2|DIO1_PIN_PAD3)
#define SIO1_PINS_TRX        (DIO1_PIN_PAD0|DIO1_PIN_PAD1)

/// Analog
#define AIO_PORT0           PORT->Group[0] // PortA
#define AIO_PORT1           PORT->Group[1] // PortB
#define AIO_PIN_VREF        PORT_PA03
#define AIO_PIN_DACOUT      PORT_PA02
#define AIO_PIN_SPEAKER     PORT_PA05
#define AIO_PIN_ADCIN       PORT_PA04 // ADC0.4
#define AIO_PIN_VBAT        PORT_PB04 // ADC1.6 
#define AIO_PIN_VIO         PORT_PB05 // ADC1.7
#define AIO_GROUP_PIN0      (AIO_PIN_VREF | AIO_PIN_DACOUT | AIO_PIN_SPEAKER | AIO_PIN_ADCIN)
#define AIO_GROUP_PIN1      (AIO_PIN_VBAT | AIO_PIN_VIO)

/// SERCOMs init

#define LCD_PORT            PORT->Group[1] // PortB
#define LCD_BL_PORT         PORT->Group[1] // PortB

#define UART_PORT           PORT->Group[1] // PortB
#define UART_PIN_RXD        PORT_PB02
#define UART_PIN_TXD        PORT_PB03
#define UART_SERCOM         SERCOM5
#define UART_IRQn           SERCOM5_2_IRQn
#define UART_IRQ_Handler    SERCOM5_2_Handler
#define UART_SERCOM_FREQ    60000000.0

#define LCD_PIN_WR          PORT_PB14   // SS, PAD 2
#define LCD_PIN_DOUT        PORT_PB12   // MOSI, PAD 0
#define LCD_PIN_CLK         PORT_PB13   // CLK, PAD 1
#define LCD_PIN_DC          PORT_PB15   // MISO(GPIO) PAD 3
#define LCD_PIN_MUX         PORT_PB08   // TC4.WO0
#define LCD_PIN_BL          PORT_PB09   // TC4.WO1
#define LCD_PIN_FMARK       PORT_PB17   // EXTINT 
#define LCD_PIN_TOUCH_INT   PORT_PB16   // EXTINT 
#define LCD_SERCOM          SERCOM4
#define LCD_IRQn            SERCOM4_2_IRQn
#define LCD_IRQ_Handler     SERCOM4_2_Handler
#define LCD_SERCOM_FREQ     120000000

#define I2C_PORT            PORT->Group[0] // PortA
#define I2C_PIN_SDA         PORT_PA22
#define I2C_PIN_SCL         PORT_PA23
#define I2C_SERCOM_FREQ     60000000.0

       
/// TC mapping

#define LCD_BL_TIMER            TC4

/// User defined recources



#endif //RESOURCES_H_INCLUDED
