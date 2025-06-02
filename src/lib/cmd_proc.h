/** \file cmd_proc.h
 *  \brief Este módulo é responsável por declarar as funções e variáveis
 * 
 *  Este módulo é responsável por interpretar comandos recebidos da UART,
 *  validar o checksum e executar ações como leitura e configuração de parâmetros
 *  como temperatura máxima ou histerese.
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */
#ifndef CMD_PROC_H
#define CMD_PROC_H


/** @brief Temperatura atual lida do sensor. */
extern int temperature;

/** @brief Temperatura máxima configurada. */
extern int max_temperature;

/**
 * @brief Manipulador de eventos UART.
 * 
 * Esta função deve ser registada como callback da UART e será chamada sempre
 * que ocorrer um evento UART, como receção de dados.
 * 
 * @param dev        Ponteiro para o dispositivo UART.
 * @param evt        Estrutura contendo informações sobre o evento UART.
 * @param user_data  Ponteiro para dados do utilizador (não utilizados aqui).
 */
void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt, void *user_data);

/**
 * @brief Inicializa o módulo de processamento de comandos.
 * 
 * Regista o callback da UART e prepara buffers ou estruturas necessárias para o funcionamento.
 */
void cmd_proc_init(void);

#endif // CMD_PROC_H

