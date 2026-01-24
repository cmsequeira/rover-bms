# BMS Constraints

## Voltage Contraints:
Over-voltage fault: V > 30 V
Under-voltage fault: V < 25 V

## Current Constraints:
Charge current limit: I > 8 A
Discharge current limit: I > 15 A

## Temperature Constraints:
Over-temp fault: T > 40 °C
Under-temp fault: T < -10 °C

## BMS States
| Condition                             | Rover Operational Mode                | BMS State     |
|---------------------------------------|---------------------------------------|---------------|
| Voltage, current or temperature fault | Any                                   | FAULT         |
| External power source detected        | Any                                   | CHARGING      |
| Active power demand detected          | Movement / Processing / Communication | DISCHARGING   |
| System powered, minimal activity      | Standby                               | IDLE          |
| Low SOC or extended inactivity        | Hibernation                           | SLEEP         |

