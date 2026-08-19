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


