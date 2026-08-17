
#ifndef SOLDIERSAFETY_MQTT_CLIENT_H
#define SOLDIERSAFETY_MQTT_CLIENT_H

#include "types.h"

void connectWiFiNonBlocking();
bool syncTimeNonBlocking();
void connectMQTTNonBlocking();

void publishTelemetry(const SensorData& data);
void publishHRVSummary(const SensorData& data);
void publishAlertEvent(const AlertEvent& event);

void printGatewayWiFiInfoOnce();

#endif
