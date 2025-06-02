/** \file RTDB.c
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 2/06/20025 
 */

#include <zephyr/kernel.h>
#include "RTDB.h"

rtdb_t rtdb = {
    .system_on = false,
    .temperature = 25,
    .max_temp = 40,
    .setpoint = 20,
    .hysteresis = 3
};

struct k_mutex rtdb_mutex;

// Init function to be called in `main()`
void rtdb_init(void) {
    k_mutex_init(&rtdb_mutex);
}

bool rtdb_get_system_on(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    bool value = rtdb.system_on;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

int rtdb_get_temperature(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.temperature;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

int rtdb_get_max_temp(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.max_temp;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

int rtdb_get_setpoint(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.setpoint;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

int rtdb_get_hysteresis(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.hysteresis;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

void rtdb_set_system_on(bool value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.system_on = value;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_temperature(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.temperature = value;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_max_temp(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.max_temp = value;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_setpoint(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.setpoint = value;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_hysteresis(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.hysteresis = value;
    k_mutex_unlock(&rtdb_mutex);
}
