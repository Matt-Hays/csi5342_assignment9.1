/*
 * Ceedling will auto-generate mock_pump_hardware.h / .c from pump_hardware.h
 * compile with: `$ ceedling test:test_pump_control` or `$ ceedling test:all`
 */

#include "unity.h"
#include "pump_control.h"
#include "mock_pump_hardware.h"

void setUp(void)
{
    // pump_init() must switch motor off
    hardware_motor_off_Expect();
    pump_init();
}

void tearDown(void) {}

void test_initial_state(void)
{
    TEST_ASSERT_EQUAL(STATE_IDLE, pump_get_state());
    TEST_ASSERT_EQUAL_UINT16(0, pump_get_rate());
    TEST_ASSERT_EQUAL_UINT32(0, pump_get_delivered_ul());
}

void test_rate_rejects_out_of_range(void)
{
    TEST_ASSERT_EQUAL(PUMP_ERR_RATE_OUT_OF_RANGE, pump_set_rate(0));
    TEST_ASSERT_EQUAL(PUMP_ERR_RATE_OUT_OF_RANGE, pump_set_rate(1500));
}

void test_start_pause_resume_stop_sequence(void)
{
    // configure a valid rate
    TEST_ASSERT_EQUAL(PUMP_OK, pump_set_rate(100));

    // starting => hardware interactions
    hardware_set_rate_Expect(100);
    hardware_motor_on_Expect();
    TEST_ASSERT_EQUAL(PUMP_OK, pump_start());
    TEST_ASSERT_EQUAL(STATE_INFUSING, pump_get_state());

    // pause
    hardware_motor_off_Expect();
    TEST_ASSERT_EQUAL(PUMP_OK, pump_pause());
    TEST_ASSERT_EQUAL(STATE_PAUSED, pump_get_state());

    // resume
    hardware_motor_on_Expect();
    TEST_ASSERT_EQUAL(PUMP_OK, pump_resume());
    TEST_ASSERT_EQUAL(STATE_INFUSING, pump_get_state());

    // stop
    hardware_motor_off_Expect();
    TEST_ASSERT_EQUAL(PUMP_OK, pump_stop());
    TEST_ASSERT_EQUAL(STATE_IDLE, pump_get_state());
}

void test_delivery_accumulation_exact_math(void)
{
    // 60 mL/h  => 1000 µL per minute
    TEST_ASSERT_EQUAL(PUMP_OK, pump_set_rate(60));

    hardware_set_rate_Expect(60);
    hardware_motor_on_Expect();
    TEST_ASSERT_EQUAL(PUMP_OK, pump_start());

    pump_tick_ms(60000); // +1 min
    TEST_ASSERT_EQUAL_UINT32(1000, pump_get_delivered_ul());

    pump_tick_ms(1800000);                                    // +30 min
    TEST_ASSERT_EQUAL_UINT32(31000, pump_get_delivered_ul()); // 31 mL total
}
