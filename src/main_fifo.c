// main.c with generated data & Human in Loop

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "bms/bms.h" // BMS interface

// Generate random float in range [min, max]
float rand_range(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

// Convert BMS state enum to string for printing
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
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize BMS input and output structures
    bms_inputs_t inputs = {0};
    bms_outputs_t outputs = {0};

    // Initialize BMS
    printf("BMS initialization starting...\n");
    bms_init(&outputs);

    // Print check for initialization complete
    printf("Power on complete. Transitioning to STANDBY.\n");

    // Simulate 1 second time steps
    const int step_ms = 1000;
    inputs.delta_time_ms = step_ms;

    // Open FIFO for reading commands
    int fd = open("/tmp/bms_cmd", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open FIFO");
        exit(1);
    }
    char cmd;

    // variables to track buttons pressed for simulating load and charger connection
    int t = 0;
    int discharge_button_on = 0;
    int charge_button_on = 0;

    // Simulate operations with various conditions
    while (1) {
        // Generate normal operating conditions with some input variation
        inputs.voltage = rand_range(MIN_VOLTAGE + 0.2f, MAX_VOLTAGE - 0.2f);
        inputs.temperature = rand_range(MIN_TEMPERATURE + 0.2f, MAX_TEMPERATURE- 0.2f);
        inputs.load_request = LOAD_MINIMAL;
        inputs.fault_reset = false;
        inputs.wake_request = false;
        // Current will be set based on load and charger state below to reflect more realistic behavior
        if (inputs.charger_connected) {
            inputs.current = rand_range(0.2f, MAX_CHARGE_CURRENT * 0.8f);
        } else if (inputs.load_request == LOAD_HIGH) {
            inputs.current = rand_range(0.5f, MAX_DISCHARGE_CURRENT * 0.8f);
        } else {
            inputs.current = rand_range(0.0f, 0.5f); // idle
        }
        // Update charger and load state based on button presses
        if (charge_button_on > 0) {
            inputs.charger_connected = true;
        } else {
            inputs.charger_connected = false;
        }
        if (discharge_button_on > 0) {
            inputs.load_request = LOAD_HIGH;
        } else {
            inputs.load_request = LOAD_MINIMAL;
        }

        // Check for user input commands from FIFO
        int n = read(fd, &cmd, 1);
        if (n > 0) {
            switch (cmd) {
                case 'c': charge_button_on = 5; discharge_button_on = 0; break;
                case 'd': discharge_button_on = 5; charge_button_on = 0; break;
                case 'w': inputs.wake_request = true; break;
                case 'r': inputs.fault_reset = true; break;
                case 's': discharge_button_on = 0; charge_button_on = 0; break;
                // Simulate random fault conditions on 'f' command
                case 'f': {
                    int rand_fault = rand() % 5;
                    switch(rand_fault) {
                        case 0: inputs.voltage = MAX_VOLTAGE + 1; break;
                        case 1: inputs.voltage = MIN_VOLTAGE - 1; break;
                        case 2: inputs.current = MAX_DISCHARGE_CURRENT + 1; break;
                        case 3: inputs.temperature = MAX_TEMPERATURE + 5; break;
                        case 4: inputs.temperature = MIN_TEMPERATURE - 5; break;
                    }
                    break;
                }
                default: break;
            }
        }

        // Decrease button timers
        if (charge_button_on > 0) {
            charge_button_on--;
        }
        if (discharge_button_on > 0) {
            discharge_button_on--;
        }

        // Process inputs and outputs based on BMS logic
        bms_run(&inputs, &outputs);

        // Print current state and key parameters
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

        // Increment time and sleep for next step
        t++;

        // Sleep for 1 second to simulate real-time operation
        sleep(1);

    }

    return 0;
}