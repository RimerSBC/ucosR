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
#ifndef BSP_H_INCLUDED
#define BSP_H_INCLUDED

#include "sam.h"

#include "stdint.h"
#include "stdbool.h"

#ifndef NULL
#define NULL (void *)0
#endif

#define _ASSERT_ENABLE_ 1
#if defined(_ASSERT_ENABLE_)
#define ASSERT(expr)  {if (!(expr)) asm("BKPT #0"); }
#else
#define ASSERT(expr) ((void) 0)
#endif

#define CONF_PIN_VAL(x) ((x)>0xffff ? (((x) >> 16) | PORT_WRCONFIG_HWSEL ) : x)

#define SYS_CLOCK_FREQ		120000000

#define VERSION         0
#define SUBVERSION      60


#define SAMD51				1
#define BOARD_USB_ENABLE	    0

#define DEF_CONF_FONT       0   // Rimer8x12 font
#define DEF_CONF_TEXT_FG    SC_GREEN
#define DEF_CONF_TEXT_BG    SC_BLACK
#define DEF_CONF_LCD_BRIGHT 60
#define DEF_CONF_LED_PWR    1
#define DEF_CONF_SPKR_MAX   4000
#define DEF_CONF_SPKR_VOL   2000
#define DEF_CONF_SPKR_SND   0
#define DEF_CONF_INTERFACE  "sys"
#define DEF_CONF_FILE_NAME  ".rimer.conf"
#define IFACE_NAME_LEN  	15
#define CONFIG_STARTUP_LEN  127

typedef enum
{
    CLK_120MHZ=0,
    CLK_60MHZ,
    CLK_12MHZ
} sys_clocks_t;


#define CLOCK_USB_ENABLE        0
#if CLOCK_USB_ENABLE
#define CONF_GCLK_USB_SRC      GCLK_PCHCTRL_GEN_GCLK3
#endif

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


#define SERCOM_CLOCKS_CONFIG() {\
        REG_GCLK_PCHCTRL3 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;} // All SERCOMs slow clock @ 12MHz

#define SERCOM_UART_CONFIG()  {\
        REG_GCLK_PCHCTRL23 = CLK_60MHZ | GCLK_PCHCTRL_CHEN;/* SERCOM2 core clock @ 60MHz  */\
        REG_MCLK_APBBMASK |= MCLK_APBBMASK_SERCOM2;\
        UART_PORT.WRCONFIG.reg = CONF_PIN_VAL(UART_PIN_RXD | UART_PIN_TXD) | PORT_WRCONFIG_PMUX(0x03) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;}

#define SERCOM_I2C_CONFIG()  {\
        REG_GCLK_PCHCTRL24 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;/* SERCOM3 core clock @ 12MHz (I2C)*/\
        REG_MCLK_APBBMASK |= MCLK_APBBMASK_SERCOM3;\
        I2C_PORT.WRCONFIG.reg = CONF_PIN_VAL(I2C_PIN_SDA | I2C_PIN_SCL) | PORT_WRCONFIG_PMUX(0x02) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;}

#define SERCOM_LCD_CONFIG()  {\
        REG_GCLK_PCHCTRL34 = CLK_120MHZ | GCLK_PCHCTRL_CHEN;/* SERCOM4 core clock @ 120MHz (SPI)*/\
        REG_MCLK_APBDMASK |= MCLK_APBDMASK_SERCOM4;\
        LCD_PORT.WRCONFIG.reg = CONF_PIN_VAL(LCD_PIN_WR | LCD_PIN_DOUT | LCD_PIN_CLK) | PORT_WRCONFIG_PMUX(0x02) | PORT_WRCONFIG_DRVSTR | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;}
        
/// TC mapping

/// TC init

#define LCD_BL_TIMER            TC4

#define TIMER_LCD_BL_CONFIG() {\
        REG_GCLK_PCHCTRL30 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;/* TC4/TC5 core clock @ 12MHz*/\
        REG_MCLK_APBCMASK |= MCLK_APBCMASK_TC4;\
        LCD_BL_PORT.WRCONFIG.reg = CONF_PIN_VAL(LCD_PIN_BL) | PORT_WRCONFIG_PMUX(0x04) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;}
        
#define TIMER_LCD_BL_DECONFIG() {\
        LCD_BL_PORT.WRCONFIG.reg = CONF_PIN_VAL(LCD_PIN_BL) | PORT_WRCONFIG_PMUX(0x00) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;\
        LCD_BL_PORT.DIRSET.reg = LCD_PIN_BL;\
        LCD_BL_PORT.OUTCLR.reg = LCD_PIN_BL;}

#define AIO_CONFIG() {\
        REG_GCLK_PCHCTRL40 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;/* ADC0 clock @ 12MHz*/\
        REG_GCLK_PCHCTRL41 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;/* ADC1 clock @ 12MHz*/\
        REG_GCLK_PCHCTRL42 = CLK_12MHZ | GCLK_PCHCTRL_CHEN;/* DAC clock @ 12MHz*/\
        REG_MCLK_APBDMASK |= MCLK_APBDMASK_ADC0 | MCLK_APBDMASK_ADC1 | MCLK_APBDMASK_DAC;\
        AIO_PORT0.WRCONFIG.reg = CONF_PIN_VAL(AIO_GROUP_PIN0) | PORT_WRCONFIG_PMUX(0x01) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;\
        AIO_PORT1.WRCONFIG.reg = CONF_PIN_VAL(AIO_GROUP_PIN1) | PORT_WRCONFIG_PMUX(0x01) | PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_WRPINCFG;}
        
typedef struct
{
    uint8_t font;       // System Font selection
    uint8_t textFG;     // terminal text foregroung colour
    uint8_t textBG;     // terminal text backgroung colour
    uint8_t bright;     // LCD backlight brightness in percentsi
    uint16_t volume;    // speaker volume
    uint8_t kbdsnd;     // Key's clicking sound
    char    iface[IFACE_NAME_LEN+1];        // Last used interface name
    char    startup[CONFIG_STARTUP_LEN+1];  // Startup command line
    uint8_t checkSum;   // config structure checksum
}_sysconf_t;

typedef struct __attribute__((packed))
{
	uint64_t AC_BIAS:2; // 1:0
	uint64_t ADC0_BIASCOMP:3; // 4:2
	uint64_t ADC0_BIASREFBUF:3; // 7:5
	uint64_t ADC0_BIASR2R:3; // 10:8
	uint64_t Reserved0:5; // 15:11
	uint64_t ADC1_BIASCOMP:3; // 18:16
	uint64_t ADC1_BIASREFBUF:3; // 21:19
	uint64_t ADC1_BIASR2R:3; // 24:22
	uint64_t Reserved1:10; // 35:25
	uint64_t USB_TRANSN:5; // 36:32
	uint64_t USB_TRANSP:5; // 41:37
	uint64_t USB_TRIM:3; // 44:42
} sw_cal_area_t;

#define rnd_init() {REG_MCLK_APBCMASK |= MCLK_APBCMASK_TRNG;TRNG->CTRLA.bit.ENABLE = 1;}
#define rnd(range) (TRNG->DATA.reg % range)

void bsp_init(void);
uint8_t conf_checksum(_sysconf_t *conf);

extern bool usbConnected;
extern _sysconf_t sysConf;
extern sw_cal_area_t *calData;
#endif //BSP_H_INCLUDED
