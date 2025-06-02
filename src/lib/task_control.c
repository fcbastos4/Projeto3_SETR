/** \file task_control.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include "task_control.h"
#include "sensor_temp.h"
#include "RTDB.h"

LOG_MODULE_REGISTER(task_control, LOG_LEVEL_INF);

#define LEDPWM_NODE DT_NODELABEL(pwm_led2)
#define PWM_PERIOD PWM_MSEC(20)  // 20 ms period (50 Hz)

static const struct pwm_dt_spec pwm_resistor = PWM_DT_SPEC_GET(LEDPWM_NODE);

//extern int temperature;         // actual temperature
//extern int max_temperature;     // temperature setpoint
//static int hysteresis = 3;      // ±1°C hysteresis band

/* Mutexes to protect shared variables */
/*static struct k_mutex temp_mutex;
static struct k_mutex max_temp_mutex;*/

#define CONTROL_INTERVAL_MS 500

static void control_thread(void *a, void *b, void *c) {
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    if (!device_is_ready(pwm_resistor.dev)) {
        LOG_ERR("PWM device not ready");
        return;
    }

    bool heating;

    while (1) {
        // int temp = 0, max = 0;

        /* Safe access to shared variables */
        /*k_mutex_lock(&temp_mutex, K_FOREVER);
        temp = temperature;
        k_mutex_unlock(&temp_mutex);

        k_mutex_lock(&max_temp_mutex, K_FOREVER);
        max = max_temperature;
        k_mutex_unlock(&max_temp_mutex);*/

        int temp = rtdb_get_temperature();
        int max = rtdb_get_max_temp();
        int hysteresis = rtdb_get_hysteresis();

        int error = max - temp;

        if(rtdb_get_system_on()) {
            // ON/OFF control with hysteresis
            if (error >= hysteresis) {
                heating = false;
            } else {
                heating = true;
            }
            printf("\r\nERROR: %d HEATING: %d\r\n",error,heating);

            uint32_t pulse = heating ? PWM_PERIOD : 0;

            int ret = pwm_set_dt(&pwm_resistor, PWM_PERIOD, pulse);
            if (ret) {
                LOG_ERR("pwm_set_dt failed: %d", ret);
            } /*else {
                LOG_INF("Heating %s | Temp: %d | Setpoint: %d",
                        heating ? "ON" : "OFF", temp, max);
            }*/
        }
        k_msleep(CONTROL_INTERVAL_MS);
    }
}

void task_control_init(void) {
    static K_THREAD_STACK_DEFINE(control_stack, 512);
    static struct k_thread control_tid;

    /* Init mutexes */
    /*k_mutex_init(&temp_mutex);
    k_mutex_init(&max_temp_mutex);*/

    k_thread_create(&control_tid,
                    control_stack,
                    K_THREAD_STACK_SIZEOF(control_stack),
                    control_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);
}
