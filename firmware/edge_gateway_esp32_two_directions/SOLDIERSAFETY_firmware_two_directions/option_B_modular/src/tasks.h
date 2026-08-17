
#ifndef SOLDIERSAFETY_TASKS_H
#define SOLDIERSAFETY_TASKS_H

void Task_AnomalyDetection(void* pvParameters);
void Task_EnvReceiver(void* pvParameters);
void Task_AlarmControl(void* pvParameters);
void Task_CloudPublish(void* pvParameters);

#endif
