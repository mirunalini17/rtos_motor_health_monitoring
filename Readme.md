# Project Overview
An RTOS-based motor health monitoring system developed using the STM32F103C8T6 microcontroller and FreeRTOS. The system monitors motor operating conditions using temperature and vibration sensor data and identifies abnormal operating conditions through threshold-based fault detection.The application uses multiple RTOS tasks to acquire sensor data, process measurements, and monitor the overall motor health status.

# Objectives
Implement multitasking using FreeRTOS.

Acquire temperature data using an LM35 sensor.

Acquire acceleration data using an MPU6050 sensor.

Calculate vibration RMS from sampled acceleration data.

Implement inter-task communication using message queues.

Identify normal, warning, and critical operating conditions.

Implement an Independent Watchdog Timer (IWDG) for firmware recovery.

# Hardware and Software Requirements

# Components

STM32F103C8T6 (Blue Pill)

LM35 temperature sensor

MPU6050 accelerometer/gyroscope module

Virtual Terminal for UART output

# Development Tools

STM32CubeIDE

FreeRTOS (through CMSIS-RTOS2 APIs)

# System Architecture
The proposed system is developed using the STM32F103C8T6 microcontroller with FreeRTOS as the underlying real-time operating system. The 
architecture consists of three independent tasks: TemperatureTask, VibrationTask, and MonitoringTask. The TemperatureTask acquires temperature 
readings from the LM35 sensor through the ADC interface, while the VibrationTask collects acceleration data from the MPU6050 sensor through 
the I2C interface and calculates the vibration RMS value. Two separate message queues are used to transfer temperature and vibration 
measurements to the MonitoringTask. The MonitoringTask processes the received sensor data, evaluates predefined warning and critical 
thresholds, and determines the overall motor health condition. UART communication through USART2 is used to display sensor measurements, fault 
conditions, and motor health status. An Independent Watchdog Timer (IWDG) is also configured to provide automatic system recovery in case of 
firmware failure.

# Methodology
The implementation begins with configuring the STM32 peripherals, including ADC, I2C, USART2, GPIO, and the Independent Watchdog Timer. 
FreeRTOS is initialized, and three tasks are created with appropriate priorities and stack allocations. The LM35 sensor readings are acquired 
periodically through ADC conversion and converted into temperature values in degrees Celsius. Simultaneously, the MPU6050 provides 
acceleration measurements along the three axes. A batch of 100 acceleration samples is collected, the mean acceleration is removed from each 
axis, and the dynamic vibration RMS value is calculated. The resulting temperature and vibration measurements are transmitted to the 
MonitoringTask using separate FreeRTOS message queues. The MonitoringTask compares the received values against configurable warning and 
critical thresholds and classifies each parameter as NORMAL, WARNING, or CRITICAL. Based on both sensor conditions, an overall motor health 
status is generated and displayed through UART. The watchdog mechanism is used to demonstrate automatic recovery when periodic refresh is 
stopped and the configured timeout expires.

# Results
 
Successful temperature acquisition and dynamic temperature variation were observed using the LM35 sensor input. The MPU6050 acceleration 
readings were acquired, and vibration RMS values were calculated from batches of 100 samples using mean-subtracted acceleration data. The 
FreeRTOS tasks executed successfully, and sensor measurements were transferred to the MonitoringTask through the configured message queues. 
The threshold-based fault detection mechanism successfully identified individual temperature and vibration warning and critical conditions. 
Combined motor health classification was also verified, demonstrating the system's ability to identify normal and abnormal operating 
conditions based on both sensor measurements. Watchdog timeout and restart behavior were also explored through simulation. These results 
demonstrate the functional operation of the proposed RTOS-based monitoring architecture in the simulation environment. Physical hardware 
implementation and validation will be carried out as the next stage of development.