
#include "bms.h"
#include "bms_state.h"
#include "bms_limits.h"

void bms_update(const bms_inputs_t *inputs, bms_outputs_t *outputs) 
{
    // Default safe outputs
    outputs->charge_enabled = false;
    outputs->discharge_enabled = false;
    outputs->fault_active = false;
    outputs->fault_flag = FAULT_NONE;

    // 1. Run INIT
    if (outputs->state == BMS_INIT) {
        outputs->state = BMS_IDLE;
        return;
    }

    // 2. Check for faults
    bool overcharge_current = inputs->charger_connected && (inputs->current > MAX_CHARGE_CURRENT);
    bool overdischarge_current = (!inputs->charger_connected) && (inputs->current > MAX_DISCHARGE_CURRENT);

    bool fault =
        (inputs->voltage > MAX_VOLTAGE) ||
        (inputs->voltage < MIN_VOLTAGE) ||
        overcharge_current ||
        overdischarge_current ||
        (inputs->temperature > MAX_TEMPERATURE) ||
        (inputs->temperature < MIN_TEMPERATURE);

    if (fault) {
        outputs->state = BMS_FAULT;
        outputs->fault_active = true;
        return;
    }

    // 3. Charging
    if (inputs->charger_connected) {
        outputs->state = BMS_CHARGING;
        outputs->charge_enabled = true;
        return;
    }

    // 4. Sleep
        if (inputs->load_request == LOAD_MINIMAL) {
            outputs->state = BMS_SLEEP;
            return;
        }

    // 5. Discharging
    if (inputs->load_request == LOAD_LOW || inputs->load_request == LOAD_MEDIUM || inputs->load_request == LOAD_HIGH) {
        outputs->state = BMS_DISCHARGING;
        outputs->discharge_enabled = true;
        return;
    }

    // 6. Idle
    if (inputs->load_request == LOAD_NONE) {
    outputs->state = BMS_IDLE;
        return;
    }
}