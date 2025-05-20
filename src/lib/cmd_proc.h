#ifndef CMD_PROC_H
#define CMD_PROC_H

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt, void *user_data);

#endif // CMD_PROC_H
