# Rover BMS Design Parameters

Using the publically available information from NASA's perseverance Mars Rover, this project will use the following design parameters for the battery.  

| Parameter                     | Value                                   |
|------------------------------|-----------------------------------------|
| Number of Batteries          | 2 Parallel Strings (8s2p architecture)  |
| Battery Chemistry            | Lithium Ion (NMC)                        |
| Nominal Voltage              | 28.8 V                                  |
| Maximum Voltage              | 32.8 V                                  |
| Minimum Voltage              | 24.0 V                                  |
| Capacity                     | 43 Ah per string                        |
| Total Energy                 | 2408 Wh                                 |
| Maximum Charge Current       | 10 A (assumption)                       |
| Maximum Discharge Current    | 22 A, 30 A pulse                        |
| Operating Temperature Range  | -20°C to +30°C                          |

*Each string can be charged independently, discharged together  

NOTE: Li-ion battery cell range is 3.0-4.2V, therefore 8 cells are used.  

# Parameter Limits

| Parameter Type | Constraint Type       | Limit            |
|----------------|--------------------|----------------|
| Voltage        | Over-voltage fault   | V > 30 V       |
| Voltage        | Under-voltage fault  | V < 25 V       |
| Current        | Charge current limit | I > 8 A        |
| Current        | Discharge current limit | I > 15 A    |
| Temperature    | Over-temp fault      | T > 40 °C      |
| Temperature    | Under-temp fault     | T < -10 °C     |
