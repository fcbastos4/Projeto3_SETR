#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>

LOG_MODULE_REGISTER(cmd_proc, LOG_LEVEL_DBG);

#define S_O_Frame '#'
#define E_O_Frame '!'

#define RECEIVE_BUFF_SIZE 128

extern const struct device *uart;  // UART deve ser inicializado noutro módulo (ex: main.c)

static uint8_t rx_buf[RECEIVE_BUFF_SIZE] = {0};
static size_t rx_buf_index = 0;

uint32_t button_states[4];
uint32_t led_states[4];

static void uart_send(const char *data) {
    int ret = uart_tx(uart, data, strlen(data), SYS_FOREVER_MS);
    if (ret) {
        LOG_ERR("UART send failed: %d", ret);
    }
}

static void process_command(const char *cmd, const char *data) {
    if (strcmp(cmd, "L") == 0) {
        for (int i = 0; i < 4; i++) {
            led_states[i] = data[i] - '0';
        }
        write_led_states(led_states);
    } else if (strcmp(cmd, "R") == 0) {
        if (strcmp(data, "A") == 0) {
            int value = read_analog_value();
            char buf[32];
            snprintf(buf, sizeof(buf), "\n\rAnalog value: %d mV", value);
            uart_send(buf);
        } else if (strcmp(data, "B") == 0) {
            read_button_states(button_states);
            char buf[64];
            snprintf(buf, sizeof(buf), "\n\rButton states: %d %d %d %d",
                     button_states[0], button_states[1], button_states[2], button_states[3]);
            uart_send(buf);
        }
    } else {
        uart_send("\n\rInvalid command");
    }
}

static void process_frame(const char *frame) {
    char cmd[2] = {0};
    char data[5] = {0};
    int frame_len = strlen(frame);

    if (frame[0] != S_O_Frame || frame[frame_len - 2] != E_O_Frame || frame[frame_len - 1] != '\r') {
        uart_send("\n\rInvalid frame");
        return;
    }

    sscanf(frame, "#%1s", cmd);

    if (strcmp(cmd, "L") == 0){
        if (frame_len != 8){
            uart_send("\n\rInvalid frame");
            return;
        }
        sscanf(frame, "#L%4s!", data);
        for(int i = 0; i < 4; i++){
            if (data[i] != '0' && data[i] != '1'){
                uart_send("\n\rInvalid frame");
                return;
            }
        }
    } else if (strcmp(cmd, "R") == 0){
        if (frame_len != 5){
            uart_send("\n\rInvalid frame");
            return;
        }
        sscanf(frame, "#R%1s!", data);
        if (data[0] != 'A' && data[0] != 'B'){
            uart_send("\n\rInvalid frame");
            return;
        }
    } else {
        uart_send("\n\rInvalid frame");
        return;
    }

    process_command(cmd, data);
}

// Public function to be used in UART callback
void cmd_proc_uart_handler(const struct device *dev, struct uart_event *evt) {
    static bool receiving_frame = false;
    switch (evt->type) {
        case UART_RX_RDY:
            for (size_t i = 0; i < evt->data.rx.len; i++) {
                char received_char = evt->data.rx.buf[evt->data.rx.offset + i];
                if (received_char != '\r')
                    printk("%c", received_char);
                else
                    printk("\n\r");

                if (received_char == S_O_Frame) {
                    receiving_frame = true;
                    rx_buf_index = 0;
                }

                if (receiving_frame && rx_buf_index < RECEIVE_BUFF_SIZE - 1) {
                    rx_buf[rx_buf_index++] = received_char;
                }

                if (received_char == '\r') {
                    rx_buf[rx_buf_index] = '\0';
                    process_frame(rx_buf);
                    receiving_frame = false;
                    rx_buf_index = 0;
                }
            }
            break;
        case UART_RX_DISABLED:
            uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100);  // Timeout in ms
            break;
        default:
            break;
    }
}
