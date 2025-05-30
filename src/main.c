/** \file main.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */

/* Includes. */
#include <zephyr/kernel.h>          /* for k_msleep() */
#include <zephyr/device.h>          /* for device_is_ready() and device structure */
#include <zephyr/devicetree.h>		/* for DT_NODELABEL() */
#include <zephyr/drivers/gpio.h>    /* for GPIO api*/
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include "lib/cmd_proc.h"
#include "lib/sensor_temp.h" 
#include "lib/task_control.h"


LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define RECEIVE_BUFF_SIZE 128
#define RECEIVE_TIMEOUT_MS 100
#define UPDATE_PERIOD_MS 500 		/* Time between temperature readings */

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));  // Ajuste conforme necessário
static uint8_t rx_buf[RECEIVE_BUFF_SIZE];

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
int desired_temp = 25;	// Desired temperature
int temperature = 25;		// Actual temperature
int max_temperature = 40;

// Thread Priorities
#define TEMP_THREAD_PRIORITY 5
#define LED_THREAD_PRIORITY  6

// Thread stack areas and data
K_THREAD_STACK_DEFINE(temp_stack, 1024);
K_THREAD_STACK_DEFINE(led_stack, 1024);
static struct k_thread temp_thread_data;
static struct k_thread led_thread_data;

// Mutex and semaphore
K_MUTEX_DEFINE(temp_mutex);
K_SEM_DEFINE(led_sem, 0, 1); // Binary semaphore

// Function to update LEDs based on temperature deviation
void update_leds() {
	printk("temperature: %d", temperature);
	printk("temperature: %d", desired_temp);

    k_mutex_lock(&temp_mutex, K_FOREVER);
    int deviation = temperature - desired_temp;
    k_mutex_unlock(&temp_mutex);

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
		printk("Warning! temperature Low!\n");
    }
    // LED4 (Warning High): ON if >2°C above temp
    else if (deviation > 2) {
        gpio_pin_set_dt(&led2, 0); // LED2 OFF
        gpio_pin_set_dt(&led3, 0); // LED3 OFF
        gpio_pin_set_dt(&led4, 1); // LED4 ON (Warning High)
		printk("Warning! temperature High!\n");
    }
}

/* Temperature Thread */
void temp_thread(void *a, void *b, void *c) {
    while (1) {
        int temp = get_temp();
        k_mutex_lock(&temp_mutex, K_FOREVER);
        temperature = temp;
        k_mutex_unlock(&temp_mutex);

        k_sem_give(&led_sem);  // Signal LED thread
        k_msleep(UPDATE_PERIOD_MS); // Sleep for configured update period
	}
}

/* LED Thread */
void led_thread(void *a, void *b, void *c) {
    while (1) {
        k_sem_take(&led_sem, K_FOREVER);
        if (led1_state) {
            update_leds();
        }
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
		k_sem_give(&led_sem);
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

	k_mutex_lock(&temp_mutex, K_FOREVER);

	desired_temp++;
	if (desired_temp <= max_temperature)
	{
		printk("Actual temperature: %d C\n", desired_temp);
	} else {
		desired_temp = max_temperature;
		printk("Maximum opereating temperature!");
	}
	
	k_mutex_unlock(&temp_mutex);
    k_sem_give(&led_sem);
	return;
}

void button4Pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_msleep(100); // Debounce

	k_mutex_lock(&temp_mutex, K_FOREVER);

    desired_temp--;
	printk("Actual temperature: %d C\n", desired_temp);

	k_mutex_unlock(&temp_mutex);
    k_sem_give(&led_sem);
	return;
}

/*
 * The main function
 */
int main(void)
{
	task_control_init();
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

	printk("System ready. Press buttons to toggle LEDs. Inicial temperature 25 C.\n");
    
    int ret1;
    if (!device_is_ready(uart)) {
        printk("UART device not ready\n");
        return 1;
    }

    // Define o callback de recepção UART
    ret1 = uart_callback_set(uart, cmd_proc_uart_handler, NULL);
    if (ret1 < 0) {
        printk("Failed to set UART callback: %d\n", ret1);
        return 1;
    }

    // Inicia a recepção UART
    ret1 = uart_rx_enable(uart, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT_MS);
    if (ret1 < 0) {
        printk("Failed to enable UART RX: %d\n", ret1);
        return 1;
    }
	printk("UART frame parser ready\r\n");

	// Start threads
    k_thread_create(&temp_thread_data, temp_stack,
                    K_THREAD_STACK_SIZEOF(temp_stack),
                    temp_thread, NULL, NULL, NULL,
                    TEMP_THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&led_thread_data, led_stack,
                    K_THREAD_STACK_SIZEOF(led_stack),
                    led_thread, NULL, NULL, NULL,
                    LED_THREAD_PRIORITY, 0, K_NO_WAIT);
		
	return 0;
}