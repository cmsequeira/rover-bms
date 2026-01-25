#include <studio.h>
#include <stdbool.h>
#include "bms.h"
#include "bms_state.h"
#include "bms_limits.h"

#define ASSERT_EQUAL(a, b, message) \
    if ((a) != (b)) { 
        printf("FAIL: %s\n", message); 
    } else { printf("PASS: %s\n", message); 
    }

void test_fault_voltage_high() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    inputs.voltage = MAX_VOLTAGE + 1; // Set voltage above max limit
    inputs.current = 0;
    inputs.temperature = 25; // Normal temperature
    inputs.charger_connected = false;
    inputs.load_requested = LOAD_MEDIUM;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_FAULT, "Overvoltage triggers FAULT");
}

void test_charging_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    inputs.voltage = 25;
    inputs.current = 0;
    inputs.temperature = 25; // Normal temperature
    inputs.charger_connected = true;
    inputs.load_requested = LOAD_HIGH;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_CHARGING, "Charger connected triggers CHARGING");
}

void test_discharge_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    inputs.voltage = 25;
    inputs.current = 2;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_requested = LOAD_MEDIUM;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_DISCHARGING, "Medium Load triggers DISCHARGING");
}

void test_sleep_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    inputs.voltage = 25;
    inputs.current = 0.1;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_requested = LOAD_MINIMAL;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_SLEEP, "Minimal load triggers SLEEP");
}

void test_idle_state() {
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    inputs.voltage = 25;
    inputs.current = 0;
    inputs.temperature = 25;
    inputs.charger_connected = false;
    inputs.load_requested = LOAD_NONE;

    bms_update(&inputs, &outputs);
    ASSERT_EQUAL(outputs.state, BMS_IDLE, "No load triggers IDLE");

}

int main() {
    test_fault_voltage_high();
    test_charging_state();
    test_discharge_state();
    test_sleep_state();
    test_idle_state();
    return 0;
}