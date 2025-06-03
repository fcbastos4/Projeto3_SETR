/** \file task_control.c
 *  \brief Implementação do controlo de aquecimento via PWM.
 * 
 *  Este módulo implementa uma thread responsável por controlar o estado da
 *  temperatura atual, setpoint e histerese, utilizando
 *  um sinal PWM para aquecer uma resitencia de aquecimento.
 * 
 *  \author Guilherme Santos, 103143  
 *  \author Francisco Bastos, 103359  
 *  \date 22/05/2025
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include "task_control.h"
#include "sensor_temp.h"
#include "RTDB.h"

LOG_MODULE_REGISTER(task_control, LOG_LEVEL_INF);

// Definição do nó PWM no Device Tree
#define LEDPWM_NODE DT_NODELABEL(pwm_led2)

// Período do PWM (20 ms = 50 Hz)
#define PWM_PERIOD PWM_MSEC(20)

// Objeto PWM configurado a partir do Device Tree
static const struct pwm_dt_spec pwm_resistor = PWM_DT_SPEC_GET(LEDPWM_NODE);

// Prioridade da thread de controlo
#define CONTROL_THREAD_PRIORITY  5

// Intervalo de tempo entre iterações da thread (em ms)
#define CONTROL_INTERVAL_MS 500


 /* Esta função corre continuamente em background, verificando se o sistema está ligado,
  e ajusta o estado do aquecedor de acordo com a diferença entre a temperatura atual
  e o setpoint, aplicando histerese.*/

static void control_thread(void *a, void *b, void *c) {
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    // Verifica se o dispositivo PWM está pronto
    if (!device_is_ready(pwm_resistor.dev)) {
        LOG_ERR("PWM device not ready");
        return;
    }

    bool heating;                   // Estado atual do PWM (ligado/desligado)
    int last_error = -999;          // Último erro calculado (para evitar prints repetidos)
    int last_temp = -999;           // Última temperatura lida (para evitar prints repetidos)

    while (1) {
        // Apenas executa controlo se o sistema estiver ligado
        if (rtdb_get_system_on() == 1) {
            int temp = rtdb_get_temperature();
            int hysteresis = rtdb_get_hysteresis();
            int setpoint = rtdb_get_setpoint();
            int max_temp = rtdb_get_max_temp();

            int error = setpoint - temp;

            // Apenas imprime log se algum valor tiver mudado
            if (error != last_error || temp != last_temp) {
                printf("\r\nERROR: %d TEMP: %d  SETPOINT: %d  HYSTERESE: %d   MAX_TEMP:%d \r\n", error, temp, setpoint, hysteresis, max_temp);
                last_error = error;
                last_temp = temp;
            }

            // Lógica de controlo com histerese
            if (error >= hysteresis) {
                heating = false;  // Desliga aquecedor
            } else {
                heating = true;   // Liga aquecedor
            }

            // Define o ciclo ativo do PWM (0 se desligado)
            uint32_t pulse = heating ? PWM_PERIOD : 0;
            int ret = pwm_set_dt(&pwm_resistor, PWM_PERIOD, pulse);
            if (ret) {
                LOG_ERR("pwm_set_dt failed: %d", ret);
            }
        }else {
            // Sistema desligado: garante PWM desligado
            int ret = pwm_set_dt(&pwm_resistor, PWM_PERIOD, PWM_PERIOD);
            if (ret) {
                LOG_ERR("pwm_set_dt failed (system off): %d", ret);
            }
        }

        // Espera antes da próxima iteração
        k_msleep(CONTROL_INTERVAL_MS);
    }
}

void task_control_init(void) {
    static K_THREAD_STACK_DEFINE(control_stack, 512);
    static struct k_thread control_tid;

    k_thread_create(&control_tid,
                    control_stack,
                    K_THREAD_STACK_SIZEOF(control_stack),
                    control_thread,
                    NULL, NULL, NULL,
                    CONTROL_THREAD_PRIORITY, 0, K_NO_WAIT);
}
