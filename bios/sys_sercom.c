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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp.h"
#include "sys_sercom.h"
#include "i2c_port.h"

typedef enum
{
    SYS_SERCOM_READY,
    SYS_SERCOM_BUSY,
    SYS_SERCOM_ERR,
} _sercom_status_t;

typedef enum
{
    SYS_SERCOM_WRITE,
    SYS_SERCOM_READ,
    SYS_SERCOM_INIT,
} _sercom_command_t;

typedef struct
{
    uint16_t id;
    uint16_t addr;
    uint16_t size;
    uint8_t *data;
    _sercom_command_t cmd;
    _sercom_status_t *stat;
} _sercom_data_t;

QueueHandle_t xSysSercom3queue = NULL;

bool sys_kbd_write(uint8_t cmd,uint8_t data)
{
    _sercom_status_t status = SYS_SERCOM_BUSY;
    _sercom_data_t qData =
    {
        .id   = I2C_IO_EXTENDER(0),
        .addr = cmd,
        .size = 1,
        .data = &data,
        .cmd  = SYS_SERCOM_WRITE,
        .stat = &status
    };
    while (xSysSercom3queue == NULL) taskYIELD();
    xQueueSend(xSysSercom3queue,&qData,portMAX_DELAY);
    while (status == SYS_SERCOM_BUSY) taskYIELD();
    return status == SYS_SERCOM_READY ? true : false;
}

bool sys_kbd_read(uint8_t cmd,uint8_t *data)
{
    _sercom_status_t status = SYS_SERCOM_BUSY;
    _sercom_data_t qData =
    {
        .id   = I2C_IO_EXTENDER(0),
        .addr = cmd,
        .size = 1,
        .data = data,
        .cmd  = SYS_SERCOM_READ,
        .stat = &status
    };
    while (xSysSercom3queue == NULL) taskYIELD();
    xQueueSend(xSysSercom3queue,&qData,portMAX_DELAY);
    while (status == SYS_SERCOM_BUSY) taskYIELD();
    return status == SYS_SERCOM_READY ? true : false;
}

bool sys_eeprom_write(uint16_t addr,uint8_t *data,uint16_t size)
{
    _sercom_status_t status;
    _sercom_data_t qData =
    {
        .id   = I2C_EEPROM(0),
        .addr = addr,
        .size = size,
        .data = data,
        .cmd  = SYS_SERCOM_WRITE,
        .stat = &status
    };
    while (size)
    {
        status = SYS_SERCOM_BUSY;
        qData.size = size > I2C_EEPROM_PAGE_SIZE ? I2C_EEPROM_PAGE_SIZE : size;
        while (xSysSercom3queue == NULL) taskYIELD();
        xQueueSend(xSysSercom3queue,&qData,portMAX_DELAY);
        while (status == SYS_SERCOM_BUSY) taskYIELD();
        if (status != SYS_SERCOM_READY) break;
        size -= qData.size;
        qData.addr += qData.size;
        qData.data += qData.size;
        vTaskDelay(5); // Write Cycle Time delay
    }
    return status == SYS_SERCOM_READY ? true : false;
}

bool sys_eeprom_read(uint16_t addr,uint8_t *data,uint16_t size)
{
    _sercom_status_t status = SYS_SERCOM_BUSY;
    _sercom_data_t qData =
    {
        .id   = I2C_EEPROM(0),
        .addr = addr,
        .size = size,
        .data = data,
        .cmd  = SYS_SERCOM_READ,
        .stat = &status
    };
    while (xSysSercom3queue == NULL) taskYIELD();
    xQueueSend(xSysSercom3queue,&qData,portMAX_DELAY);
    while (status == SYS_SERCOM_BUSY) taskYIELD();
    return status == SYS_SERCOM_READY ? true : false;
}

void sys_sercom3_task(void *vParam)
{
    _sercom_data_t qData;
    SERCOM_I2C_CONFIG();
    i2c_port_init(SERCOM3,I2C_SPEED_400KHZ);
    xSysSercom3queue = xQueueCreate(SYS_SERCOM3_QUEUE_LEN,sizeof(_sercom_data_t));
    while (true)
    {
        xQueueReceive(xSysSercom3queue,&qData,portMAX_DELAY);
        switch(qData.cmd)
        {
        case SYS_SERCOM_WRITE:
            *qData.stat = (i2c_write(SERCOM3, qData.id, qData.addr, qData.data, qData.size) == I2C_ERR_NONE) ? SYS_SERCOM_READY : SYS_SERCOM_ERR;
            break;
        case SYS_SERCOM_READ:
            *qData.stat = (i2c_read(SERCOM3, qData.id, qData.addr, qData.data, qData.size) == I2C_ERR_NONE) ? SYS_SERCOM_READY : SYS_SERCOM_ERR;
            break;
        case SYS_SERCOM_INIT:
            break;
        }
    }
}
