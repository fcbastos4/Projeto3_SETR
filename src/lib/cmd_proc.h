/** \file cmd_proc.h
 *  \brief
 * 
 *  ...
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */
#ifndef CMD_PROC_H
#define CMD_PROC_H

#include <zephyr/device.h>

extern int temperature;
extern int max_temperature;

/**
 * @brief Manipulador de eventos UART que deve ser chamado a partir do callback da UART.
 * 
 * @param dev  Ponteiro para o dispositivo UART.
 * @param evt  Evento UART recebido.
 * @param user_data  Dados do utilizador (não usados neste contexto).

 */
void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt, void *user_data);

#endif // CMD_PROC_H

