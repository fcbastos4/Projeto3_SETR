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
    rtdb_init();
    rtdb_set_system_on(false);
    rtdb_set_temperature(25);
    rtdb_set_max_temp(30);
    rtdb_set_hysteresis(1);
    memset(uart_last_sent, 0, sizeof(uart_last_sent));
}

void test_command_C_valid_checksum(void) {
    const char *frame = "#C067!\r"; // checksum("C") = 67
    process_frame(frame);
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
    TEST_ASSERT_TRUE(strstr(uart_last_sent, "#s005232!"));
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

void test_rtdb_system_on(void) {
    rtdb_set_system_on(true);
    TEST_ASSERT_TRUE(rtdb_get_system_on());
    rtdb_set_system_on(false);
    TEST_ASSERT_FALSE(rtdb_get_system_on());
}

void test_rtdb_temperature(void) {
    rtdb_set_temperature(55);
    TEST_ASSERT_EQUAL_INT(55, rtdb_get_temperature());
}

void test_rtdb_max_temp(void) {
    rtdb_set_max_temp(99);
    TEST_ASSERT_EQUAL_INT(99, rtdb_get_max_temp());
}

void test_rtdb_setpoint(void) {
    rtdb_set_setpoint(77);
    TEST_ASSERT_EQUAL_INT(77, rtdb_get_setpoint());
}

void test_rtdb_hysteresis(void) {
    rtdb_set_hysteresis(8);
    TEST_ASSERT_EQUAL_INT(8, rtdb_get_hysteresis());
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
    RUN_TEST(test_rtdb_system_on);
    RUN_TEST(test_rtdb_temperature);
    RUN_TEST(test_rtdb_max_temp);
    RUN_TEST(test_rtdb_setpoint);
    RUN_TEST(test_rtdb_hysteresis);

    return UNITY_END();
}

