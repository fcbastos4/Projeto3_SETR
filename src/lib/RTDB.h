/** \file RTDB.h
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 2/06/20025 
 */

#ifndef RTDB_H
#define RTDB_H
#include <stdbool.h>

#define rtdb_get_max_temperature rtdb_get_max_temp
#define rtdb_set_max_temperature rtdb_set_max_temp

// Database structure
typedef struct {
    bool system_on;
    int temperature;
    int max_temp;
    int setpoint;
    int hysteresis;
} rtdb_t;

// Public access
extern rtdb_t rtdb;

// Mutex to protect the RTDB
extern struct k_mutex rtdb_mutex;

void rtdb_init(void);

// Getters
bool rtdb_get_system_on(void);
int rtdb_get_temperature(void);
int rtdb_get_max_temp(void);
int rtdb_get_setpoint(void);
int rtdb_get_hysteresis(void);

// Setters
void rtdb_set_system_on(bool value);
void rtdb_set_temperature(int value);
void rtdb_set_max_temp(int value);
void rtdb_set_setpoint(int value);
void rtdb_set_hysteresis(int value);

#endif // RTDB_H
