#include <stdio.h>
#include <stdbool.h>
#include "bms.h"
#include "bms_state.h"
#include "bms_limits.h"

#define ASSERT_EQUAL(a, b, message) if ((a) != (b)) { printf("FAIL: %s\n", message); } else { printf("PASS: %s\n", message); }

void test_fault_voltage_high() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = MAX_VOLTAGE + 1; // Set voltage above max limit
    inputs.current = 0;
    inputs.temperature = 25; // Normal temperature
    inputs.charger_connected = false;
    inputs.load_request = LOAD_MEDIUM;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_FAULT, "Overvoltage triggers FAULT");
}

void test_multiple_faults() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = MAX_VOLTAGE + 5;  // Overvoltage
    inputs.current = MAX_CHARGE_CURRENT + 1; // Overcurrent
    inputs.temperature = MAX_TEMPERATURE + 1; // Overtemperature
    inputs.charger_connected = true;
    inputs.load_request = LOAD_HIGH;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_FAULT, "Multiple faults trigger FAULT");
    if ((outputs.fault_flag & FAULT_OVERVOLTAGE) && 
        (outputs.fault_flag & FAULT_OVERCURRENT) &&
        (outputs.fault_flag & FAULT_OVERTEMPERATURE)) {
        printf("PASS: Multiple faults detected correctly\n");
    } else {
        printf("FAIL: Multiple faults not detected\n");
    }
}

void test_charging_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = 25;
    inputs.current = 0;
    inputs.temperature = 25; // Normal temperature
    inputs.charger_connected = true;
    inputs.load_request = LOAD_HIGH;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_CHARGING, "Charger connected triggers CHARGING");
}

void test_discharge_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = 25;
    inputs.current = MAX_DISCHARGE_CURRENT;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_request = LOAD_MEDIUM;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_DISCHARGING, "Medium Load triggers DISCHARGING");
}

void test_sleep_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = 25;
    inputs.current = 0.1;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_request = LOAD_MINIMAL;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_SLEEP, "Minimal load triggers SLEEP");
}

void test_wake_from_sleep() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    // Force sleep
    inputs.voltage = 25;
    inputs.current = 0;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_request = LOAD_MINIMAL;
    inputs.wake_request = false;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_SLEEP, "Minimal load triggers SLEEP");

    // Wake request
    inputs.wake_request = true;
    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_STANDBY, "Wake request exits SLEEP to STANDBY");
}


void test_standby_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs;
    bms_init(&outputs);

    inputs.voltage = 25;
    inputs.current = 0;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_request = LOAD_NONE;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_STANDBY, "No load triggers STANDBY");

}

int main() {
    test_fault_voltage_high();
    test_multiple_faults();
    test_charging_state();
    test_discharge_state();
    test_sleep_state();
    test_standby_state();
    return 0;
}