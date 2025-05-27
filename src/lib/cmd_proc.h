#ifndef CMD_PROC_H
#define CMD_PROC_H

#include <zephyr/device.h>

/**
 * @brief Manipulador de eventos UART que deve ser chamado a partir do callback da UART.
 * 
 * @param dev  Ponteiro para o dispositivo UART.
 * @param evt  Evento UART recebido.
 * @param user_data  Dados do utilizador (não usados neste contexto).

 */
extern int temperature;
extern int max_temperature;


void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt, void *user_data);

#endif // CMD_PROC_H

