/** \file RTDB.h
 * \brief Módulo responsável pela Real-Time Database (RTDB).
 *
 * Este módulo fornece uma base de dados partilhada entre tarefas para armazenar
 * parâmetros do sistema como estado, temperatura, temperatura máxima, setpoint e histerese.
 * O acesso à base de dados é protegido por um mutex para garantir a integridade dos dados
 * em ambientes concorrentes.
 * 
 * \author Guilherme Santos
 * \author Francisco Bastos
 * \date 2/06/2025
 */

#ifndef RTDB_H
#define RTDB_H

#ifdef UNIT_TEST
#include "../../test/zyphyr_mock.h"
#else
#include <zephyr/kernel.h>
#endif

/**
 * \brief Macro para compatibilidade de nomes antigos.
 */
#define rtdb_get_max_temperature rtdb_get_max_temp
#define rtdb_set_max_temperature rtdb_set_max_temp

/**
 * \brief Estrutura que representa a base de dados partilhada (RTDB).
 */
typedef struct {
    bool system_on;     /**< Indica se o sistema está ativo ou desligado */
    int temperature;    /**< Temperatura atual lida do sensor */
    int max_temp;       /**< Temperatura máxima configurada */
    int setpoint;       /**< Setpoint para controlo da temperatura */
    int hysteresis;     /**< Valor de histerese para controlo ON/OFF */
} rtdb_t;

/**
 * \brief Instância global da base de dados.
 */
extern rtdb_t rtdb;

/**
 * \brief Mutex para proteger o acesso concorrente à RTDB.
 */
extern struct k_mutex rtdb_mutex;

/**
 * \brief Inicializa a RTDB e o mutex associado.
 */
void rtdb_init(void);

/**
 * \brief Lê o estado atual do sistema (ligado/desligado).
 * \return true se o sistema estiver ligado, false caso contrário.
 */
bool rtdb_get_system_on(void);

/**
 * \brief Lê a temperatura atual.
 * \return Temperatura lida do sensor.
 */
int rtdb_get_temperature(void);

/**
 * \brief Lê a temperatura máxima configurada.
 * \return Temperatura máxima permitida.
 */
int rtdb_get_max_temp(void);

/**
 * \brief Lê o setpoint configurado para controlo.
 * \return Valor de setpoint.
 */
int rtdb_get_setpoint(void);

/**
 * \brief Lê o valor de histerese atual.
 * \return Valor de histerese.
 */
int rtdb_get_hysteresis(void);

/**
 * \brief Define o estado do sistema.
 * \param value true para ligado, false para desligado.
 */
void rtdb_set_system_on(bool value);

/**
 * \brief Define a temperatura atual lida do sensor.
 * \param value Temperatura atual.
 */
void rtdb_set_temperature(int value);

/**
 * \brief Define a temperatura máxima permitida.
 * \param value Nova temperatura máxima.
 */
void rtdb_set_max_temp(int value);

/**
 * \brief Define o setpoint de temperatura.
 * \param value Novo valor de setpoint.
 */
void rtdb_set_setpoint(int value);

/**
 * \brief Define o valor de histerese.
 * \param value Novo valor de histerese.
 */
void rtdb_set_hysteresis(int value);

#endif // RTDB_H
