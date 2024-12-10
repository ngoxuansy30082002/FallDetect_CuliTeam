#ifndef MQTT_H /* include guards */
#define MQTT_H
#include "define.h"

typedef struct
{
} mod_mqtt_data;

void MQTT_Init();
void MQTT_Task();
void mqttPublish(bool alarm);
extern mod_mqtt_data modMqttDt;

#endif /* MQTT_H */