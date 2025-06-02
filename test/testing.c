#define DEFINE_UART_MOCKS
#include "zyphyr_mock.h"
#include "unity.h"
#include "../src/lib/cmd_proc.h"
#include "../src/lib/RTDB.h"
#include <string.h>



extern void process_frame(const char *frame);
extern char uart_last_sent[128]; // Exposed or mocked buffer from uart_send

void tearDown(void) {}

void setUp(void) {
    // Reset RTDB values
    rtdb_set_temperature(25);
    rtdb_set_max_temp(30);
    rtdb_set_hysteresis(1);
    memset(uart_last_sent, 0, sizeof(uart_last_sent));
}

void test_command_C_valid_checksum(void) {
    const char *frame = "#C067!\r"; // checksum("C") = 67
    process_frame(frame);
    printf("UART Last Sent: %s\n", uart_last_sent);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#c025067!"));
}

void test_command_C_invalid_checksum(void) {
    const char *frame = "#C025!\r"; // invalid checksum
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Es"));
}

void test_command_C_invalid_frame(void) {
    const char *frame = "#C02567!\r"; // invalid frame length
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Ef171"));
}

void test_command_M_valid(void) {
    const char *frame = "#M030224!\r"; // 30 = temp, 224 = checksum("M030")
    process_frame(frame);
    TEST_ASSERT_EQUAL_INT(30, rtdb_get_max_temperature());
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#m030224!"));
}

void test_command_M_invalid_checksum(void) {
    const char *frame = "#M030000!\r"; // incorrect checksum
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Es"));
}
void test_command_M_invalid_frame(void) {
    const char *frame = "#M0302245!\r"; // invalid frame length
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Ef171"));
}

void test_command_S_valid(void) {
    const char *frame = "#S005232!\r"; // 5 = hyst, 232 = checksum("S005")
    process_frame(frame);
    TEST_ASSERT_EQUAL_INT(5, rtdb_get_hysteresis());
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#m005232!"));
}

void test_command_S_invalid_checksum(void) {
    const char *frame = "#S005000!\r"; // incorrect checksum
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Es"));
}
void test_command_S_invalid_frame(void) {
    const char *frame = "#S0052325!\r"; // invalid frame length
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Ef171"));
}

void test_command_D(void) {
    const char *frame = "#D!\r"; // Invalid command
    process_frame(frame);
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#Ei174"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_command_C_valid_checksum);
    RUN_TEST(test_command_C_invalid_checksum);
    RUN_TEST(test_command_C_invalid_frame);
    RUN_TEST(test_command_M_valid);
    RUN_TEST(test_command_M_invalid_checksum);
    RUN_TEST(test_command_M_invalid_frame);
    RUN_TEST(test_command_S_valid);
    RUN_TEST(test_command_S_invalid_checksum);
    RUN_TEST(test_command_S_invalid_frame);
    RUN_TEST(test_command_D);

    return UNITY_END();
}

