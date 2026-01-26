#include "bms.h"
#include "bms_state.h"
#include "bms_limits.h"

void bms_init(bms_outputs_t *outputs) 
{
    outputs->state = BMS_INIT;
    outputs->charge_enabled = false;
    outputs->discharge_enabled = false;
    outputs->fault_active = false;
    outputs->fault_flag = FAULT_NONE;
}

void bms_update(const bms_inputs_t *inputs, bms_outputs_t *outputs) 
{
    // Default safe outputs
    outputs->charge_enabled = false;
    outputs->discharge_enabled = false;
    outputs->fault_active = false;
    outputs->fault_flag = FAULT_NONE;

    // 1. Run INIT
    if (outputs->state == BMS_INIT) {
        outputs->state = BMS_STANDBY;
        return;
    }

    // 2. Check for faults
    bool overcharge_current = inputs->charger_connected && (inputs->current > MAX_CHARGE_CURRENT);
    bool overdischarge_current = (!inputs->charger_connected) && (inputs->current > MAX_DISCHARGE_CURRENT);

    // Voltage Faults
    if (inputs->voltage > MAX_VOLTAGE) {
        outputs->fault_flag |= FAULT_OVERVOLTAGE;
    }

    if (inputs->voltage < MIN_VOLTAGE) {
        outputs->fault_flag |= FAULT_UNDERVOLTAGE;
    }

    // Current Faults
    if (overcharge_current || overdischarge_current) {
        outputs->fault_flag |= FAULT_OVERCURRENT;
    }

    // Temperature Faults
    if (inputs->temperature > MAX_TEMPERATURE) {
        outputs->fault_flag |= FAULT_OVERTEMPERATURE;
    }

    if (inputs->temperature < MIN_TEMPERATURE) {
        outputs->fault_flag |= FAULT_UNDERTEMPERATURE;
    }

    // Check if any faults were detected
    if (outputs->fault_flag != FAULT_NONE) {
        outputs->fault_active = true;
        outputs->state = BMS_FAULT;
        return;
    }

    // 3. Charging
    if (inputs->charger_connected) {
        outputs->state = BMS_CHARGING;
        outputs->charge_enabled = true;
        return;
    }

    // 4. Wake from Sleep
    if (outputs->state == BMS_SLEEP && inputs->wake_request) {
        outputs->state = BMS_STANDBY;
        return;
    }

    // 5. Sleep
    if (inputs->load_request == LOAD_MINIMAL && !inputs->wake_request) {
        outputs->state = BMS_SLEEP;
        return;
    }

    // 6. Discharging
    if (inputs->load_request == LOAD_LOW || inputs->load_request == LOAD_MEDIUM || inputs->load_request == LOAD_HIGH) {
        outputs->state = BMS_DISCHARGING;
        outputs->discharge_enabled = true;
        return;
    }

    // 7. Standby
    if (inputs->load_request == LOAD_NONE) {
        outputs->state = BMS_STANDBY;
        return;
    }
}