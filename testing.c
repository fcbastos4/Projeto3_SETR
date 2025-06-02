#include "../unity/unity.h"
#include "cmd_proc.h"
#include "RTDB.h"

// Mocked UART buffer
char uart_output[128];

// Sobrescrever uart_send
void __wrap_uart_send(const char *data) {
    snprintf(uart_output, sizeof(uart_output), "%s", data);
}

// Testa o comando C (consulta de temperatura)
void test_cmd_proc_C_command_should_respond_with_temperature(void) {
    rtdb_set_temperature(25);

    // "#C025!" + \r
    const char *frame = "#C025!\r";
    cmd_proc_uart_handler(NULL, &(struct uart_event){
        .type = UART_RX_RDY,
        .data.rx = { .len = strlen(frame), .offset = 0 }
    }, NULL);

    TEST_ASSERT_TRUE(strstr(uart_output, "#c025") != NULL);
}

// Testa o comando M (set max temperature)
void test_cmd_proc_M_command_should_set_max_temperature(void) {
    const char *frame = "#M040070!\r";  // M040 checksum: 040+M = 077 → %256 = 077
    cmd_proc_uart_handler(NULL, &(struct uart_event){
        .type = UART_RX_RDY,
        .data.rx = { .len = strlen(frame), .offset = 0 }
    }, NULL);

    TEST_ASSERT_EQUAL(40, rtdb_get_temperature());
}

// Testa comando inválido
void test_cmd_proc_invalid_command_should_return_Ei(void) {
    const char *frame = "#X999!\r";
    cmd_proc_uart_handler(NULL, &(struct uart_event){
        .type = UART_RX_RDY,
        .data.rx = { .len = strlen(frame), .offset = 0 }
    }, NULL);

    TEST_ASSERT_TRUE(strstr(uart_output, "#Ei") != NULL);
}

int main(void) {
    UNITY_BEGIN();

    test_cmd_proc_C_command_should_respond_with_temperature();
    test_cmd_proc_M_command_should_set_max_temperature();
    test_cmd_proc_invalid_command_should_return_Ei();

    return UNITY_END();
}
