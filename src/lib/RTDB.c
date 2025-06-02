/** 
 * @file RTDB.c
 * @brief Implementação da Real-Time Database (RTDB) partilhada entre tarefas.
 *
 * Este módulo implementa uma base de dados central utilizada para partilhar e sincronizar
 * variáveis de estado e controlo entre diferentes tarefas do sistema. O acesso às variáveis
 * é protegido por um mutex para evitar condições de corrida.
 * 
 * @author Guilherme Santos, 103143
 * @author Francisco Bastos, 103359
 * @date 2/06/2025
 */

#ifdef UNIT_TEST
#include "../../test/zyphyr_mock.h"
#include "RTDB.h"
#else
#include <zephyr/kernel.h>
#include "RTDB.h"
#include <stdbool.h>
#endif

// Instância global da RTDB com valores por defeito
rtdb_t rtdb = {
    .system_on = false,     // Sistema desligado por defeito
    .temperature = 25,      // Temperatura inicial (simulada)
    .max_temp = 40,         // Temperatura máxima permitida
    .setpoint = 35,         // Setpoint desejado
    .hysteresis = 3         // Histerese padrão
};

// Mutex usado para proteger o acesso concorrente à RTDB
struct k_mutex rtdb_mutex;

// Inicializa o mutex da RTDB. Deve ser chamada uma vez no arranque do sistema.
void rtdb_init(void) {
    k_mutex_init(&rtdb_mutex);
}

// Retorna o estado atual do sistema (ligado/desligado)
bool rtdb_get_system_on(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    bool value = rtdb.system_on;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

// Retorna a temperatura atual
int rtdb_get_temperature(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.temperature;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

// Retorna a temperatura máxima configurada
int rtdb_get_max_temp(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.max_temp;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

// Retorna o valor de setpoint
int rtdb_get_setpoint(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.setpoint;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

// Retorna o valor de histerese atual
int rtdb_get_hysteresis(void) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    int value = rtdb.hysteresis;
    k_mutex_unlock(&rtdb_mutex);
    return value;
}

// Define o estado do sistema (ligado/desligado)
void rtdb_set_system_on(bool value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.system_on = value;
    k_mutex_unlock(&rtdb_mutex);
}

// Define a temperatura atual
void rtdb_set_temperature(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.temperature = value;
    k_mutex_unlock(&rtdb_mutex);
}

// Define a temperatura máxima
void rtdb_set_max_temp(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.max_temp = value;
    k_mutex_unlock(&rtdb_mutex);
}

// Define o valor de setpoint
void rtdb_set_setpoint(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.setpoint = value;
    k_mutex_unlock(&rtdb_mutex);
}

// Define o valor de histerese
void rtdb_set_hysteresis(int value) {
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    rtdb.hysteresis = value;
    k_mutex_unlock(&rtdb_mutex);
}
