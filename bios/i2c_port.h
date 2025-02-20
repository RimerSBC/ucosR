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
#ifndef I2C_DRV_INCLUDED
#define I2C_DRV_INCLUDED

#include "bsp.h"

#define I2C_EEPROM_ID	0xA0
#define I2C_8BIT_ADDR  	(0x00 << 8)
#define I2C_16BIT_ADDR  (0x01 << 8)

#define I2C_BUFFER_SIZE 96
#define I2C_PORT1    	0
#define I2C_PORT2    	1

#define I2C_TIMEOUT  	100


typedef enum
{
	I2C_SPEED_100KHZ=0,        /**< Speed set to 100KHz (slowest available). */
	I2C_SPEED_400KHZ,           /**< Speed set to 400KHz */
	I2C_SPEED_1MHZ,                /**< Speed set to 1MHz */
} i2c_speed_t;

typedef enum
{
	I2C_ERR_NONE = 0,             /**< No errors have occured */
	I2C_ERR_NODEV,					/**< No response on address request*/
	I2C_ERR_NOACK,                  /**< Error - No acknowledgement from device*/
	I2C_ERR_BUSERR,                /**< Error - Bus error*/
	I2C_ERR_SCLHOLD,                /**< Error - Clock HOLD*/
	I2C_ERR_SDAHOLD,                /**< Error - Data HOLD*/
	I2C_ERR_PARAM,                /**< Error - Wrong parameter*/
	I2C_ERR_COUNT                /**< Error codes counter*/
} i2c_err_t;

typedef enum
{
	I2C_STAT_UNKNOWN = 0,  /**< I2C bus is in unknown state */
	I2C_STAT_IDLE,                    /**< I2C bus is idle */
	I2C_STAT_OWNER,              /**< Device is I2C bus owner*/
	I2C_STAT_BUSY                  /**< I2C bus is busy */
} i2c_stat_t;

typedef enum
{
	I2C_SM_UNKNOWN = 0,    /**< I2C state machine is in unknown state */
	I2C_SM_START,                  /**< I2C bus communications started */
	I2C_SM_ADDR_LSB,           /**< I2C bus LSB of address selection */
	I2C_SM_ADDR_MSB,           /**< I2C bus MSB of address selection */
	I2C_SM_DATA,                    /**< I2C bus data receiving/transmitting */
	I2C_SM_DONE                    /**< I2C communications finished */
} i2c_sm_t;



void i2c_port_init(Sercom *port,i2c_speed_t speed);

i2c_err_t i2c_write(Sercom *port,uint16_t id, uint16_t addr,uint8_t *data,uint16_t size);

i2c_err_t i2c_read(Sercom *port,uint16_t id, uint16_t addr,uint8_t *data,uint16_t size);

/// Write data without adress sending
i2c_err_t i2c_bare_write(Sercom *port, uint8_t id, uint8_t *buf, uint16_t size);
/// Read data without adress sending
i2c_err_t i2c_bare_read(Sercom *port, uint8_t id, uint8_t *buf, uint16_t size);

/// Reset the SERCOM with previously set values
void i2c_port_reset(Sercom *port);

#endif // I2C_DRV_INCLUDED
