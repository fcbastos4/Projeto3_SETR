/** \file i2c.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 *  \date 22/05/20025 
 */

#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h> /* Required for  I2C */


#include "i2c.h"

#define ERR_FATAL -1   /* If I2C fails ...*/

#define TC74_CMD_RTR 0x00   /* Read temperature command */
#define TC74_CMD_RWCR 0x01  /* Read/write configuration register */

/* I2C device vars and defines */
#define I2C0_NID DT_NODELABEL(tc74sensor)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NID);

/* Other defines*/
#define UPDATE_PERIOD_MS 500 /* Time between temperature readings */

/* 
 * Main() function
 */
int get_temp(void)
{
    int ret=0; /* General return variable */   
    uint8_t temp=0; /* Temperature value (raw read from sensor)*/

    printk("I2C demo - reads the temperature of a TC74 temperature sensor connected to I2C0\n\r");

    if (!device_is_ready(dev_i2c.bus)) {
	    printk("I2C bus %s is not ready!\n\r",dev_i2c.bus->name);
	    return ERR_FATAL;
    } else {
        printk("I2C bus %s, device address %x ready\n\r",dev_i2c.bus->name, dev_i2c.addr);
    }    
    
    /* Write (command RTR) to set the read address to temperature */
    /* Only necessary if a config done before (not the case), but let's stay in the safe side */
    ret = i2c_write_dt(&dev_i2c, TC74_CMD_RTR, 1);
    if(ret != 0){
        printk("Failed to write to I2C device at address %x, register %x \n\r", dev_i2c.addr ,TC74_CMD_RTR);
    }
    
    /* Read temperature register */       
    ret = i2c_read_dt(&dev_i2c, &temp, sizeof(temp));
    if(ret != 0){
	    printk("Failed to read from I2C device at address %x, register  at Reg. %x \n\r", dev_i2c.addr,TC74_CMD_RTR);      
    }

    printk("Last temperature reading is %d \n\r",temp);
    
    return temp;
    /* Pause   
    k_msleep(UPDATE_PERIOD_MS); 
    */   
 
}