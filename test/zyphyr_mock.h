#pragma once




#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef DEFINE_UART_MOCKS
char uart_last_sent[128] = {0};
const struct device *uart = NULL;
#else
extern char uart_last_sent[128];
extern const struct device *uart;
#endif

/* ===================== Tipos e Macros Básicos ===================== */

#define K_FOREVER 0
#define SYS_FOREVER_MS 0
#define LOG_MODULE_REGISTER(x, ...)
#define LOG_INF(...)
#define LOG_ERR(...)
#define LOG_DBG(...)

/* ===================== Estruturas simuladas ===================== */

struct k_mutex {
    int dummy;
};

struct device {
    const char *name;
};

/* Simulação de eventos UART */
// ...existing code...

enum {
    UART_RX_RDY,
    UART_RX_DISABLED
};

struct uart_event {
    int type;
    struct {
        struct {
            char *buf;
            size_t offset;
            size_t len;
        } rx;
    } data;
};

// ...existing code...

#define UART_RX_RDY       1
#define UART_RX_DISABLED  2

struct uart_config {
    int baudrate;
    int parity;
    int stop_bits;
    int data_bits;
    int flow_ctrl;
};

#define UART_CFG_PARITY_NONE    0
#define UART_CFG_STOP_BITS_1    0
#define UART_CFG_DATA_BITS_8    0
#define UART_CFG_FLOW_CTRL_NONE 0

/* ===================== Funções simuladas ===================== */
static inline void uart_send(const char *msg) {
    strncpy(uart_last_sent, msg, sizeof(uart_last_sent) - 1);
    uart_last_sent[sizeof(uart_last_sent) - 1] = '\0';
}

static inline void k_mutex_init(struct k_mutex *m) {}
static inline void k_mutex_lock(struct k_mutex *m, int timeout) {}
static inline void k_mutex_unlock(struct k_mutex *m) {}

static inline void k_sleep(int ms) {}

static inline int device_is_ready(const struct device *dev) { return 1; }

static inline const struct device* DEVICE_DT_GET(int x) { return NULL; }
#define DT_NODELABEL(x) 0

static inline void uart_configure(const struct device *dev, const struct uart_config *cfg) {}
static inline void uart_callback_set(const struct device *dev, void *cb, void *user_data) {}
static inline void uart_rx_enable(const struct device *dev, uint8_t *buf, size_t size, int timeout) {}
static inline void uart_rx_disable(const struct device *dev) {}

static inline int uart_tx(const struct device *dev, const uint8_t *buf, size_t len, int timeout) {
    size_t copy_len = len < sizeof(uart_last_sent) - 1 ? len : sizeof(uart_last_sent) - 1;
    memcpy(uart_last_sent, buf, copy_len);
    uart_last_sent[copy_len] = '\0';
    return 0;
}
static inline void printk(const char *fmt, ...) {}
