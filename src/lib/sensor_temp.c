/** \file sensor_temp.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */

#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h> /* Required for  I2C */
#include "sensor_temp.h"

#define ERR_FATAL -1   /* If I2C fails ...*/

#define TC74_CMD_RTR 0x00   /* Read temperature command */
#define TC74_CMD_RWCR 0x01  /* Read/write configuration register */
// extern int temperature;         // actual temperature
/* I2C device vars and defines */
#define I2C0_NID DT_NODELABEL(tc74sensor)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NID);

/* Other defines*/


/* 
 * Main() function
 */
int get_temp(void)
{
    int ret=0; /* General return variable */   
    uint8_t temp=0; /* Temperature value (raw read from sensor)*/

    if (!device_is_ready(dev_i2c.bus)) {
	    printk("I2C bus %s is not ready!\n\r",dev_i2c.bus->name);
	    return ERR_FATAL;
    } /*else {
        printk("I2C bus %s, device address %x ready\n\r",dev_i2c.bus->name, dev_i2c.addr);
    }*/    
    
    /* Write (command RTR) to set the read address to temperature */
    /* Only necessary if a config done before (not the case), but let's stay in the safe side */
    ret = i2c_write_dt(&dev_i2c, TC74_CMD_RTR, 1);
    if(ret != 0){
        printk("Failed to write to I2C device at address %x, register %x \n\r", dev_i2c.addr ,TC74_CMD_RTR);
    }

    // while(1) {
        /* Read temperature register */       
        ret = i2c_read_dt(&dev_i2c, &temp, sizeof(temp));
        if(ret != 0){
	        printk("Failed to read from I2C device at address %x, register  at Reg. %x \n\r", dev_i2c.addr,TC74_CMD_RTR);      
        }

        printk("Last temperature reading is %d \n\r",temp);
        
        return temp;
            
    //}
}

/*
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "i2c.h"

#define TC74_NODE DT_ALIAS(tc74sensor)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(TC74_NODE);

#define TC74_CMD_RTR 0x00

int get_temp(uint8_t *temp)
{
    uint8_t cmd = TC74_CMD_RTR;
    int ret;

    if (!device_is_ready(dev_i2c.bus)) {
        printk("I2C bus %s not ready\n", dev_i2c.bus->name);
        return -1;
    }

    ret = i2c_write_dt(&dev_i2c, &cmd, 1);
    if (ret != 0) {
        printk("Failed to send RTR command\n");
        return -1;
    }

    ret = i2c_read_dt(&dev_i2c, temp, 1);
    if (ret != 0) {
        printk("Failed to read temperature\n");
        return -1;
    }

    return 0;
}*/