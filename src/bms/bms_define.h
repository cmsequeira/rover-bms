#ifndef BMS_DEFINE_H
#define BMS_DEFINE_H

// Limits for the Battery Management System (BMS) based on requirements document and typical lithium-ion battery characteristics
#define MAX_VOLTAGE 30.0f
#define MIN_VOLTAGE 25.0f
#define MAX_CHARGE_CURRENT 8.0f
#define MAX_DISCHARGE_CURRENT 15.0f
#define MAX_TEMPERATURE 40.0f
#define MIN_TEMPERATURE -10.0f

// Fault codes
#define FAULT_NONE             0u          // No fault
#define FAULT_OVERVOLTAGE      (1u << 0)   // Voltage too high
#define FAULT_UNDERVOLTAGE     (1u << 1)   // Voltage too low
#define FAULT_OVERCURRENT      (1u << 2)   // Current too high
#define FAULT_OVERTEMPERATURE  (1u << 3)   // Temperature too high
#define FAULT_UNDERTEMPERATURE (1u << 4)   // Temperature too low

// Sleep delay in milliseconds
#define sleep_delay_ms 30000u

#endif // BMS_DEFINE_H