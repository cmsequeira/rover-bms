// main.c with generated data

#include <stdio.h>
#include "unistd.h"
#include "bms/bms.h"
#include <stdlib.h>
#include <time.h>

float rand_range(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

static const char* read_state(bms_state_t state) {
    switch (state) {
        case BMS_INIT:        return "INIT";
        case BMS_STANDBY:     return "STANDBY";
        case BMS_CHARGING:    return "CHARGING";
        case BMS_DISCHARGING: return "DISCHARGING";
        case BMS_SLEEP:       return "SLEEP";
        case BMS_FAULT:       return "FAULT";
        default:              return "UNKNOWN";
    }
}

int main(void) {
    srand(time(NULL));
    
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    printf("BMS initialization starting...\n");
    bms_init(&outputs);

    printf("Power on complete. Transitioning to STANDBY.\n");

    const int step_ms = 1000; // 1 second tick
    inputs.delta_time_ms = step_ms;

    for (int t = 0; t < 78; t++) {
        inputs.voltage = rand_range(MIN_VOLTAGE + 0.2f, MAX_VOLTAGE - 0.2f);
        inputs.current = rand_range(0.5f, MAX_DISCHARGE_CURRENT * 0.8f);
        inputs.temperature = rand_range(MIN_TEMPERATURE + 0.2f, MAX_TEMPERATURE- 0.2f);
        inputs.charger_connected = false;
        inputs.load_request = LOAD_MINIMAL;
        inputs.fault_reset = false;
        inputs.wake_request = false;

        // 0–4s: STANDBY
        if (t < 5) {
            // already minimal load
        }

        // 5–9s: DISCHARGING
        else if (t < 10) {
            inputs.load_request = LOAD_HIGH;
        }

        // 10–12s: STANDBY
        else if (t < 13) {
            inputs.load_request = LOAD_MINIMAL;
        }

        // 13–17s: CHARGING
        else if (t < 18) {
            inputs.charger_connected = true;
        }

        // 18–22s: FAULT (overvoltage)
        else if (t < 23) {
            inputs.voltage = MAX_VOLTAGE + 1.0f;
        }

        // 23–52s: STANDBY (fault reset + idle)
        else if (t < 53) {
            inputs.fault_reset = (t == 23);
        }

        // 53–62s: SLEEP (auto-entered)
        else if (t < 63) {
            // stay asleep
        }

        // 63s: WAKE
        else if (t == 63) {
            inputs.wake_request = true;
        }

        // 64–68s: STANDBY after wake
        else if (t < 69) {
            // normal standby
        }

        bms_run(&inputs, &outputs);

        printf(
            "t=%02d | V=%4.1f | I=%4.1f | T=%4.1f | charger=%d | load=%d | fault=%d | state=%-11s\n",
            t,
            inputs.voltage,
            inputs.current,
            inputs.temperature,
            inputs.charger_connected,
            inputs.load_request,
            outputs.fault_active,
            read_state(outputs.state)
        );

        sleep(1);

    }

    return 0;
}