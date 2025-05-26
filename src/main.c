#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include "lib/cmd_proc.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define RECEIVE_BUFF_SIZE 128
#define RECEIVE_TIMEOUT_MS 100

const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));  // Ajuste conforme necessário
static uint8_t rx_buf[RECEIVE_BUFF_SIZE];

int main(void) {
    int ret;

    if (!device_is_ready(uart)) {
        printk("UART device not ready\n");
        return 1;
    }

    // Define o callback de recepção UART
    ret = uart_callback_set(uart, cmd_proc_uart_handler, NULL);
    if (ret < 0) {
        printk("Failed to set UART callback: %d\n", ret);
        return 1;
    }

    // Inicia a recepção UART
    ret = uart_rx_enable(uart, rx_buf, sizeof(rx_buf), RECEIVE_TIMEOUT_MS);
    if (ret < 0) {
        printk("Failed to enable UART RX: %d\n", ret);
        return 1;
    }

    printk("UART frame parser ready. Send frames like '#L1010!\\r' or '#RA!\\r'.\n");

    return 0;
}
