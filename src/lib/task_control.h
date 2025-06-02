/** \file task_control.h
 *  \brief Este módulo é responsável por declarar funções relacionadas ao controlo do sistema.
 * 
 *  Este módulo lida com a inicialização e o controlo dos periféricos de saída, como GPIOs e PWM,
 *  usados para ativar ou desativar elementos de controlo como aquecedores com base nos valores
 *  de temperatura definidos na aplicação.
 * 
 *  \author Guilherme Santos, 103143  
 *  \author Francisco Bastos, 103359  
 *  \date 22/05/2025
 */

#ifndef TASK_CONTROL_H
#define TASK_CONTROL_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

/**
 * \brief Inicializa o módulo de controlo.
 * 
 * Configura os dispositivos necessários ( PWM ) para o controlo dos atuadores
 * responsáveis pelo sistema de aquecimento. Esta função deve ser chamada no arranque do sistema.
 */
void task_control_init(void);

#endif // TASK_CONTROL_H
