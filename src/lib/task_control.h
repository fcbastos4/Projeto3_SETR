/** \file task_control.h
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */

#ifndef TASK_CONTROL_H
#define TASK_CONTROL_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

void task_control_init(void);

#endif // TASK_CONTROL_H
