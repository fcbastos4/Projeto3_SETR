/** \file main.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 *  \date 22/05/20025 
 */

/* Includes. */
#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/

#include "lib/i2c.h" 

/* Get led0 and button0 node IDs. Refer to the DTS file */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)
#define BUT0_NODE DT_ALIAS(sw0)
#define BUT1_NODE DT_ALIAS(sw1)
#define BUT3_NODE DT_ALIAS(sw3)

/* Get the device pointer, pin number, and configuration flags for led0 and button 0. A build error on this line means your board is unsupported. */
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led4 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec but1 = GPIO_DT_SPEC_GET(BUT0_NODE, gpios);
static const struct gpio_dt_spec but2 = GPIO_DT_SPEC_GET(BUT1_NODE, gpios);
static const struct gpio_dt_spec but4 = GPIO_DT_SPEC_GET(BUT3_NODE, gpios);

static bool led1_state = false; // LED1 state (false = off, true = on)
static int desired_temp = 25;	// Desired temperture
static int temperture = 25;		// Actual temperture

// Function to update LEDs based on temperature deviation
void update_leds() {
    int deviation = temperture - desired_temp;

    // LED2 (Normal): ON if within ±2°C of temp
    if (deviation >= -2 && deviation <= 2) {
        gpio_pin_set_dt(&led2, 1); // LED2 ON (Normal)
        gpio_pin_set_dt(&led3, 0); // LED3 OFF
        gpio_pin_set_dt(&led4, 0); // LED4 OFF
		printk("Normal State.\n");
    }
    // LED3 (Warning Low): ON if >2°C below temp
    else if (deviation < -2) {
        gpio_pin_set_dt(&led2, 0); // LED2 OFF
        gpio_pin_set_dt(&led3, 1); // LED3 ON (Warning Low)
        gpio_pin_set_dt(&led4, 0); // LED4 OFF
		printk("Warning! Temperture Low!\n");
    }
    // LED4 (Warning High): ON if >2°C above temp
    else if (deviation > 2) {
        gpio_pin_set_dt(&led2, 0); // LED2 OFF
        gpio_pin_set_dt(&led3, 0); // LED3 OFF
        gpio_pin_set_dt(&led4, 1); // LED4 ON (Warning High)
		printk("Warning! Temperture High!\n");
    }
}

/* Define the callback function */
void button1Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_msleep(100); // Debounce delay 

    gpio_pin_toggle_dt(&led1);
	led1_state = !led1_state; // Toggle state tracking

	if (led1_state) {
        printk("Power On. ");		// estão com muito delay
		update_leds();
    } else {
        printk("Power Off\n");		// !!!!!!!!!!!!!!!!!!!!!
		gpio_pin_set_dt(&led2, 0);  // LED2 OFF
        gpio_pin_set_dt(&led3, 0);  // LED3 OFF
        gpio_pin_set_dt(&led4, 0);  // LED4 OFF
    }
	return;
}

void button2Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_msleep(100); // Debounce

	temperture++;
	printk("Actual tempertrure: %d C\n", temperture);
	update_leds();

	return;
}

void button4Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_msleep(100); // Debounce

    temperture--;
	printk("Actual tempertrure: %d C\n", temperture);
	update_leds();

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
	if (!device_is_ready(led1.port) || !device_is_ready(led2.port) || !device_is_ready(led3.port) || !device_is_ready(led4.port) || !device_is_ready(but1.port) || !device_is_ready(but2.port) || !device_is_ready(but4.port)) {
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
	ret = gpio_pin_configure_dt(&led4, GPIO_OUTPUT_INACTIVE);
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
	gpio_init_callback(&button1CbData, button1Pressed, BIT(but1.pin));

    ret = gpio_pin_interrupt_configure_dt(&but2, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button2CbData, button2Pressed, BIT(but2.pin));

    ret = gpio_pin_interrupt_configure_dt(&but4, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button4CbData, button4Pressed, BIT(but4.pin));
	
	/* Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(but1.port, &button1CbData);
    gpio_add_callback(but2.port, &button2CbData);
    gpio_add_callback(but4.port, &button4CbData);

	printk("System ready. Press buttons to toggle LEDs. Inicial temperture 25 C.\n");
		
	return 0;
}