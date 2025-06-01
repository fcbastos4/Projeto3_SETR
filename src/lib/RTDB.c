// rtdb.c - Real-Time Database for shared variables with mutex protection

#include "RTDB.h"
#include "sensor_temp.h"
#include <zephyr/kernel.h>

// Internal data
int temperature = 25;
int max_temperature = 40;
int hysteresis = 1;
int desired_temp = 35;	// Desired temperature

// Mutex for protecting access to shared variables
static struct k_mutex rtdb_mutex;

void rtdb_init(void) {
    k_mutex_init(&rtdb_mutex);
}

int rtdb_get_temperature(void) {
    int value = get_temp();
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    value = temperature;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

void rtdb_set_temperature(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    temperature = value;
    k_mutex_unlock(&rtdb_mutex);
}

int rtdb_get_max_temperature(void) {
    int value;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    value = max_temperature;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

void rtdb_set_max_temperature(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    max_temperature = value;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_hysteresis(int value){
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    hysteresis = value;
    k_mutex_unlock(&rtdb_mutex);

}
int rtdb_get_hysteresis(void) {
    int value;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    value = hysteresis;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

void rtdb_set_desired_temperature(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    desired_temp = value;
    k_mutex_unlock(&rtdb_mutex);
}
int rtdb_get_desired_temperature(void) {
    int value;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    value = desired_temp;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}