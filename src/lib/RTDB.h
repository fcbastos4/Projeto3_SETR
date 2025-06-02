<<<<<<< HEAD
// rtdb.h
=======
/** \file RTDB.h
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 2/06/20025 
 */
>>>>>>> alteracoes-mutex

#ifndef RTDB_H
#define RTDB_H

<<<<<<< HEAD
void rtdb_init(void);

int rtdb_get_temperature(void);
void rtdb_set_temperature(int value);

int rtdb_get_max_temperature(void);
void rtdb_set_max_temperature(int value);

void rtdb_set_hysteresis(int value);
int rtdb_get_hysteresis(void);

void rtdb_set_desired_temperature(int value);
int rtdb_get_desired_temperature(void);
=======
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
>>>>>>> alteracoes-mutex

#endif // RTDB_H
