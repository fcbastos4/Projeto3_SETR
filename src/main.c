/* Includes. */
#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/

/* Get led0 and button0 node IDs. Refer to the DTS file */
#define LED0_NODE DT_ALIAS(led0)
#define LED0_NODE DT_ALIAS(led1)
#define LED0_NODE DT_ALIAS(led2)
#define LED0_NODE DT_ALIAS(led3)
#define BUT0_NODE DT_ALIAS(sw0)
#define BUT0_NODE DT_ALIAS(sw1)
#define BUT0_NODE DT_ALIAS(sw3)

/* Get the device pointer, pin number, and configuration flags for led0 and button 0. A build error on this line means your board is unsupported. */
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec but1 = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);
static const struct gpio_dt_spec but2 = GPIO_DT_SPEC_GET(BUT1_NODE, gpios);
static const struct gpio_dt_spec but4 = GPIO_DT_SPEC_GET(BUT3_NODE, gpios);

/* Define the callback function */
void button1Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led1);

	return;
}

void button2Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led2);

	return;
}

void button4Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_pin_toggle_dt(&led3);

	return;
}

/*
 * The main function
 */
int main(void)
{
	int ret = 0;
	
	static struct gpio_callback button1CbData; // Define variable for holding callback data
    static struct gpio_callback button2CbData; // Define variable for holding callback data
    static struct gpio_callback button4CbData; // Define variable for holding callback data 

	/* Check if devices are ready */
	if (!device_is_ready(led.port) || !device_is_ready(but.port)) {
		return 0;
	}	

	/* Configure the GPIO pins for input/output and set active logic */
	/* Note that the devicetree activates the internal pullup and sets the active low flag */
	/*   so an external resistor is not needed and pressing the button causes a logic level of 1*/
	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
    ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}
    ret = gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&but1, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}
    ret = gpio_pin_configure_dt(&but2, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}
    ret = gpio_pin_configure_dt(&but4, GPIO_INPUT);
	if (ret < 0) {
		return 0;
	}

	/* Set interrupt */
	ret = gpio_pin_interrupt_configure_dt(&but1, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button1CbData, button1Pressed, BIT(but.pin));

    ret = gpio_pin_interrupt_configure_dt(&but2, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button2CbData, button2Pressed, BIT(but.pin));

    ret = gpio_pin_interrupt_configure_dt(&but4, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button4CbData, button4Pressed, BIT(but.pin));
	
	/* Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(but.port, &button1CbData);
    gpio_add_callback(but.port, &button2CbData);
    gpio_add_callback(but.port, &button4CbData);

	/* 
 	* No need for a while(1) at the end of the main. 
	* In Zephyr main() is a thread that can terminate without resetting the system 
	* and there is an idle() thread that is called when there are not other tasks/ints ready
	* The idle() thread activates the power saving modes, so it should be left running when possible
 	*/ 	

	return 0;
}