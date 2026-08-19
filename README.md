# SOLDIER SAFETY Soldier Health and Safety Monitoring System

SOLDIER SAFETY is an edge-cloud soldier safety monitoring prototype designed to detect early health risks, fall events, fatigue conditions, and unsafe air-quality exposure in construction or industrial working environments. The system combines wearable sensing, local edge decision-making, ESP-NOW environmental communication, AWS IoT cloud ingestion, backend classification, data storage, and supervisor alerting.

It includes firmware, hardware documentation, cloud configuration, Lambda processing logic, test events, architecture documents, phase summaries, and validation screenshots.

---

## Project Overview

The project addresses the problem of soldier safety monitoring in environments where fatigue, falls, oxygen-related abnormalities, and particulate-matter exposure may occur. SOLDIER SAFETY uses an edge-first architecture: urgent safety decisions are made locally on the ESP32 gateway, while the cloud layer is responsible for centralized logging, validation, and notification.

The system monitors four groups of indicators:

| Monitoring Area | Main Data | Purpose |
|---|---|---|
| Biometric condition | Heart rate, SpO2, IBI, HRV indicators | Detect oxygen drop, abnormal heart rate, and fatigue risk |
| Motion condition | Acceleration, gyroscope, SVM, posture change | Detect possible fall events |
| Environmental condition | PM1.0, PM2.5, PM10, AQI | Detect unsafe air-quality exposure |
| Alert state | Edge alarm level, fall flag, risk category | Communicate urgent soldier-safety events |

The prototype is not a certified medical or industrial safety device. It is an academic proof-of-concept that demonstrates an integrated sensing, edge-processing, cloud-logging, and alerting workflow.

![System workflow](docs/figures/system_workflow.png)

---

## End-to-End Architecture

![End-to-end architecture](docs/architecture/assets/end_to_end_architecture.png)

The complete workflow is:

```text
Soldier and environment
        ↓
MAX30102, MPU6050, APM10 PM sensor
        ↓
ESP32 edge gateway and ESP-NOW environmental node
        ↓
Local risk assessment and buzzer alarm
        ↓
MQTT over TLS through AWS IoT Core
        ↓
AWS Lambda processing
        ↓
DynamoDB record, S3 historical log, SNS email alert
```

The edge device does not wait for the cloud before triggering urgent local alarms. This design reduces response latency and keeps the most safety-critical decision close to the soldier.

---

## System Components

| Layer | Component | Role |
|---|---|---|
| Wearable edge layer | XIAO ESP32-C3 / ESP32-class gateway | Runs FreeRTOS tasks, reads sensors, performs local risk logic, publishes MQTT payloads |
| Biometric sensing | MAX30102 | Measures heart rate, SpO2, and inter-beat interval data |
| Motion sensing | MPU6050 | Provides acceleration and gyroscope data for fall detection |
| Local alarm | Buzzer and cancel button | Alerts the soldier and allows acknowledgement for lower-level alarms |
| Environmental node | ESP32S NodeMCU / ESP32 DevKit + APM10 | Reads particulate-matter data and sends packets by ESP-NOW |
| Cloud ingestion | AWS IoT Core | Receives MQTT telemetry, HRV summaries, and alert events |
| Backend processing | AWS Lambda | Normalizes payloads and classifies soldier risk level |
| Storage | DynamoDB and S3 | Stores processed records and historical logs |
| Notification | SNS | Sends email alerts for serious or emergency states |

![Hardware schematic](hardware/schematics/schematic.png)

---

## Repository Structure

```text
Soldier_Health_and_Fatigue_Monitoring/
├── cloud/
│   ├── aws_iot/                 # AWS IoT policy, rule, and cloud workflow documentation
│   └── lambda/                  # Lambda processor and backend documentation
├── docs/
│   ├── architecture/            # System architecture, communication, and power-management docs
│   ├── figures/                 # General project figures
│   └── reports/                 # Report placeholder (source reports removed for anonymized submission)
├── firmware/
│   ├── edge_gateway_esp32_two_directions/
│   │   └── SOLDIERSAFETY_firmware_two_directions/
│   │       ├── option_A_single_file/  # Compact demonstration firmware
│   │       └── option_B_modular/      # Modular firmware for maintainability
│   ├── environmental_node_espnow/     # ESP32 + APM10 ESP-NOW node
│   └── micropython_prototype/         # Early MicroPython prototype
├── hardware/
│   ├── hardware_design.md       # Hardware design explanation
│   └── schematics/              # Circuit/schematic image
├── phases/                      # Phase 1, Phase 2, and Phase 3 summaries
├── results/                     # Demo screenshots and validation evidence
├── tests/                       # Lambda test events and expected behavior
├── LICENSE
└── README.md
```

---

## Main Workflows

### 1. Communication Workflow

![Communication workflow](docs/architecture/assets/communication_workflow.png)

The system uses two communication levels:

| Link | Protocol | Purpose |
|---|---|---|
| Environmental node → edge gateway | ESP-NOW | Sends PM1.0, PM2.5, PM10, AQI, sequence number, and node status |
| Edge gateway → AWS IoT Core | MQTT over TLS | Sends telemetry, HRV summaries, and alert events |
| AWS IoT Core → Lambda | IoT Rule | Triggers cloud-side processing |
| Lambda → DynamoDB/S3/SNS | AWS SDK | Stores records and sends supervisor notifications |

### 2. Edge Algorithm Pipeline

![Edge algorithm pipeline](docs/architecture/assets/edge_algorithm_pipeline.png)

The gateway evaluates multiple risk sources:

| Risk Type | Main Logic |
|---|---|
| SpO2 risk | Warning below 90%, critical below 80% |
| Heart-rate risk | Bradycardia below 50 BPM, tachycardia above 120 BPM |
| Fatigue risk | RMSSD below 20 ms for repeated windows; severe exhaustion when RMSSD is below 15 ms and BPM is above 110 |
| Fall risk | Free-fall phase, impact phase, posture change, and stillness confirmation |
| Environmental risk | AQI warning above 75 and danger above 150 |

Fall detection is intentionally multi-stage to reduce false alarms. A fall is confirmed only when the motion sequence satisfies free-fall, impact, posture-change, and inactivity/stillness conditions within the configured timing window.

### 3. Cloud Processing Workflow

![Cloud workflow](cloud/aws_iot/end_to_end_workflow.png)

The Lambda function receives telemetry or alert payloads and converts them into a standardized risk state:

| Level | State | Meaning | Notification |
|---:|---|---|---|
| 0 | `NORMAL` | No abnormal condition detected | No |
| 1 | `WARNING` | Early abnormal signal requiring monitoring | Optional / deployment-specific |
| 2 | `DANGER` | Serious condition requiring supervisor attention | Yes |
| 3 | `EMERGENCY` | Critical condition requiring immediate response | Yes |

Processed events are saved for traceability. Email notifications are used only for serious or critical events in the validation tests.

---

## Firmware Overview

### Edge Gateway Firmware

Path:

```text
firmware/edge_gateway_esp32_two_directions/SOLDIERSAFETY_firmware_two_directions/
```

This firmware runs the wearable edge gateway. It performs sensor acquisition, fall detection, fatigue estimation, local alarm control, ESP-NOW environmental reception, and MQTT publishing.

Two equivalent implementation directions are included:

| Option | Path | Best Use |
|---|---|---|
| Option A: Single-file firmware | `option_A_single_file/soldiersafety_edge_freertos_aws.ino` | Presentation, walkthrough, quick debugging |
| Option B: Modular firmware | `option_B_modular/` | Clean repository submission, extension, maintainability |

Both versions follow the same system behavior. Option A is easier to explain during a live demo, while Option B is better for long-term development because the code is separated into modules such as sensors, fall detection, HRV, alerts, MQTT, ESP-NOW, and FreeRTOS tasks.

Default MQTT topics:

```text
soldier/W001/telemetry
soldier/W001/hrv
soldier/W001/alert
```

Default edge timing and threshold examples:

| Parameter | Value |
|---|---:|
| MPU6050 read interval | 20 ms |
| MAX30102 read interval | 1000 ms |
| Cloud publish interval | 5000 ms |
| Alert cooldown | 30000 ms |
| Free-fall threshold | 4.9 m/s² |
| Impact threshold | 29.4 m/s² |
| Posture-change threshold | 35° |
| Fall latch duration | 30000 ms |

### Environmental Node Firmware

Path:

```text
firmware/environmental_node_espnow/
```

This firmware runs on an ESP32S NodeMCU or ESP32 DevKit connected to an APM10 particulate-matter sensor. It reads PM1.0, PM2.5, and PM10 values, calculates a prototype PM2.5-based AQI, and sends an `EnvPacket` to the gateway through ESP-NOW.

Key configuration values are located near the top of:

```text
firmware/environmental_node_espnow/esp32_apm10_espnow_node.ino
```

Before uploading, verify:

```cpp
#define ESPNOW_WIFI_CHANNEL 13
#define USE_BROADCAST false
uint8_t GATEWAY_MAC[6] = { ... };
#define PM_RX_PIN 16
#define PM_TX_PIN 17
#define PM_BAUD 1200
```

The ESP-NOW Wi-Fi channel and gateway MAC address must match the gateway Serial Monitor output.

---

## Cloud Backend Overview

### AWS IoT Core

Path:

```text
cloud/aws_iot/
```

This folder documents the AWS IoT configuration, including the IoT policy, topic structure, and rule-based forwarding workflow. The gateway publishes JSON payloads to AWS IoT Core through MQTT over TLS.

### Lambda Processor

Path:

```text
cloud/lambda/
```

The Lambda function:

1. normalizes incoming payloads;
2. extracts soldier, biometric, environmental, and alarm fields;
3. classifies the condition into `NORMAL`, `WARNING`, `DANGER`, or `EMERGENCY`;
4. stores processed results in DynamoDB and S3;
5. sends SNS email notifications for serious or emergency states.

## Future Work

Recommended next improvements include:

1. add configurable soldier profiles and adaptive thresholds;
2. enable ESP-NOW peer encryption;
3. improve battery measurement and runtime estimation;
4. add dashboard visualization for supervisors;
5. support multiple soldiers and multiple environmental nodes;
6. strengthen payload schema validation before cloud processing;
7. evaluate false positives and detection latency using repeated physical tests;
8. improve enclosure design for wearable stability and sensor contact quality.


