/** \file cmd_proc.c
 *  \brief Este módulo é responsável por implementar as funções e variáveis
 * 
 *  Este módulo é responsável por interpretar comandos recebidos da UART,
 *  validar o checksum e executar ações como leitura e configuração de parâmetros
 *  como temperatura máxima ou histerese.
 * 
 *  \author Guilherme Santos, 103143
 * 	\author Francisco Bastos, 103359
 *  \date 22/05/20025 
 */


#ifdef UNIT_TEST
#include "../../test/zyphyr_mock.h"
#include <ctype.h>
#include <stdlib.h>
#include "RTDB.h"  // Incluído apenas para testes
#else
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cmd_proc.h"
#include "task_control.h"
#include "RTDB.h"
#endif

LOG_MODULE_REGISTER(cmd_proc, LOG_LEVEL_DBG);

#define S_O_Frame '#'       // Carácter de início de frame
#define E_O_Frame '!'       // Carácter de fim de frame
#define RECEIVE_BUFF_SIZE 128  // Tamanho do buffer de receção UART

extern const struct device *uart;

static struct k_mutex uart_mutex;           // Mutex para proteger envios UART
static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0}; // Buffer de receção de dados
static size_t rx_buf_index = 0;             // Índice atual do buffer

#ifndef UNIT_TEST
// Envia uma string pela UART, protegendo com mutex
void uart_send(const char *data) {
    k_mutex_lock(&uart_mutex, K_FOREVER);
    int ret = uart_tx(uart, data, strlen(data), SYS_FOREVER_MS);
    k_mutex_unlock(&uart_mutex);
    if (ret) {
        LOG_ERR("UART send failed: %d", ret);
    }
}
#endif

// Calcula o checksum somando os bytes do comando e dos dados
uint8_t compute_checksum(const char *cmd, const char *data) {
    uint8_t checksum = 0;

    for (size_t i = 0; i < strlen(cmd); i++) {
        checksum += cmd[i];
    }

    for (size_t i = 0; i < strlen(data); i++) {
        checksum += data[i];
    }

    return (uint8_t)(checksum % 256);
}

// Envia um ACK/NACK com código de estado e checksum
void send_ack(const char *code) {
    char buf[32];
    uint8_t cs = compute_checksum(code, "");

    snprintf(buf, sizeof(buf), "\r\n#%s%03d\r\n", code, cs);
    uart_send(buf);
}

// Processa comandos válidos: C, M, S
void process_command(const char *cmd, const char *data) {

    if (strcmp(cmd, "C") == 0) {
        // Comando para leitura de temperatura atual
        char reply[64];
        int temp = rtdb_get_temperature();  // Lê temperatura da RTDB
        char checksum[4];
        char cs_received[4] = {0};

        strncpy(cs_received, data, 3); 
        snprintf(checksum, sizeof(checksum), "%03d", temp);

        uint8_t cs = compute_checksum("C", "");  // Checksum só do comando
        int cs_input = atoi(cs_received);

        if(cs != cs_input) {
            send_ack("Es");  // Checksum errado
        } else {
            snprintf(reply, sizeof(reply), "\r\n#c%s%03d!\r\n", checksum, cs);
            uart_send(reply);
        }

    } else if(strcmp(cmd, "M") == 0) {
        // Comando para definir temperatura máxima
        char reply[64];
        char temp_str[4] = {0};
        char cs_received[4] = {0};

        strncpy(temp_str, data, 3); 
        strncpy(cs_received, data + 3, 3); 

        uint8_t cs = compute_checksum("M", temp_str);
        int cs_input = atoi(cs_received);
        int temp_input = atoi(temp_str);

        rtdb_set_max_temp(temp_input);  // Atualiza na RTDB

        if(cs != cs_input) {
            send_ack("Es");  // Checksum errado
        } else {
            snprintf(reply, sizeof(reply), "\r\n#m%03d%03d!\r\n", temp_input, cs);
            uart_send(reply);
        }

    } else if(strcmp(cmd, "S") == 0) {
        // Comando para definir histerese
        char reply[64];
        char histerese_str[4] = {0};
        char cs_received[4] = {0};

        strncpy(histerese_str, data, 3); 
        strncpy(cs_received, data + 3, 3); 

        uint8_t cs = compute_checksum("S", histerese_str);
        int cs_input = atoi(cs_received);
        int histerese_input = atoi(histerese_str);

        rtdb_set_hysteresis(histerese_input);  // Atualiza na RTDB

        if(cs != cs_input) {
            send_ack("Es");  // Checksum errado
        } else {
            snprintf(reply, sizeof(reply), "\r\n#m%03d%03d!\r\n", histerese_input, cs);
            uart_send(reply);
        }

    } else {
        send_ack("Ei");  // Comando inválido
    }
}

// Processa um frame UART completo após validação
void process_frame(const char *frame) {
    char cmd[2] = {0};
    char data[16] = {0};
    int frame_len = strlen(frame);

    sscanf(frame, "#%1s!", cmd);  // Lê comando base

    if (strcmp(cmd, "M") == 0){
        sscanf(frame, "#%1s%6s!", cmd, data);
        if (frame_len != 10 || strspn(data, "0123456789") != 6) {
            send_ack("Ef");  // Erro de framing
            return;
        }
    }
    else if (strcmp(cmd, "C") == 0) {
        sscanf(frame, "#%1s%3s!", cmd, data);
        if (frame_len != 7 || strspn(data, "0123456789") != 3) {
            send_ack("Ef");  // Erro de framing
            return;
        }
    }
    else if (strcmp(cmd, "S") == 0) {
        sscanf(frame, "#%1s%6s!", cmd, data);
        if (frame_len != 10 || strspn(data, "0123456789") != 6) {
            send_ack("Ef");  // Erro de framing
            return;
        }
    }
    else {
        send_ack("Ei");  // Comando inválido
        return;
    }

    process_command(cmd, data);  // Passa para execução do comando
}

// Callback do driver UART para eventos de receção
void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt, void *user_data) {
    static bool receiving_frame = false;

    switch (evt->type) {
        case UART_RX_RDY:
            // Leitura de dados disponíveis
            for (size_t i = 0; i < evt->data.rx.len; i++) {
                char received_char = evt->data.rx.buf[evt->data.rx.offset + i];
                printk("%c", received_char);  // Debug UART

                if (received_char == S_O_Frame) {
                    receiving_frame = true;
                    rx_buf_index = 0;
                }

                if (receiving_frame && rx_buf_index < RECEIVE_BUFF_SIZE - 1) {
                    rx_buf[rx_buf_index++] = received_char;
                }

                if (received_char == '\r') {
                    rx_buf[rx_buf_index] = '\0';  // Termina string
                    process_frame(rx_buf);        // Processa frame
                    receiving_frame = false;
                    rx_buf_index = 0;
                }
            }
            break;

        case UART_RX_DISABLED:
            // Reativa a receção após desativação
            uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100);
            break;

        default:
            break;
    }
}

// Inicializa recursos do módulo (mutex, etc.)
void cmd_proc_init(void) {
    k_mutex_init(&uart_mutex);
}