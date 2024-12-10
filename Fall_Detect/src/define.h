#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MAX30100_PulseOximeter.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include <MPU6050.h>
#include <math.h> // để dùng hàm sqrt

#include "main.h"
#include "module_oled.h"
#include "module_mpu.h"
#include "module_gpio.h"
#include "module_pox.h"
#include "module_buzzer.h"
#include "cc.h"
#include "module_mqtt.h"

#define PIN_STOP_ALARM 8   // GPIO 0 for reset button
#define PIN_OLED_ONOFF 7      // GPIO 6: interrupt
#define PIN_LED_PWM 10         // GPIO 1 for LED
#define PIN_BUZZER 1         // Pin connected to buzzer
#define PIN_WAKEUP GPIO_NUM_6 // GPIO5 for wake-up button
#define PIN_SDA 2
#define PIN_SCL 3
#define PIN_ADC 4 // GPIO4 kết nối với cảm biến điện áp

#define POX_FINGER_TIMEOUT 20000
#define VARIATION_ACCEL_THRESHOLD 5.0f
#define VARIATION_GYRO_THRESHOLD 50
#define VARIATION_HEARBEAT_THRESHOLD 90
#define WAIT_HEARBEAT_TIMEOUT 10000
#define WAIT_FALLDETECT_TIMEOUT 50000
#define MAX_COUNTER_FALLDETECT 5
