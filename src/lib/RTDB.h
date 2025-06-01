// rtdb.h

#ifndef RTDB_H
#define RTDB_H

void rtdb_init(void);

int rtdb_get_temperature(void);
void rtdb_set_temperature(int value);

int rtdb_get_max_temperature(void);
void rtdb_set_max_temperature(int value);

void rtdb_set_hysteresis(int value);
int rtdb_get_hysteresis(void);

void rtdb_set_desired_temperature(int value);
int rtdb_get_desired_temperature(void);

#endif // RTDB_H
