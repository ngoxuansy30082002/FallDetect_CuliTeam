#include "define.h"

main_data mainDt;

unsigned long startCounter = 0;
static void _enterLightSleepMode();

void setup()
{
  Serial.begin(9600);
  Wire.begin(PIN_SDA, PIN_SCL); // SDA = GPIO3, SCL = GPIO2 trên ESP32-C3
  // cc_init();
  MQTT_Init();
  MOD_GPIO_Init();
  MOD_MPU_Init();
  MOD_OLED_Init();
  MOD_POX_Init();
  MOD_BUZZER_Init();

  mainDt.state = IDLE;
  turn_off_buzzer();
  mqttPublish(false);
}
void loop()
{
  Serial.printf("%.2f %.2f %.2f\r\n", modMpuDt.accelVar, modMpuDt.gyroVar, modPoxDt.avgHeartRate);
  // cc_task();
  MOD_GPIO_Task();
  MOD_MPU_Task();
  MOD_OLED_Task();
  MOD_POX_Task();
  MOD_BUZZER_Task();
  MQTT_Task();

  switch (mainDt.state)
  {
  case IDLE:
    if (modMpuDt.accelVar > VARIATION_ACCEL_THRESHOLD && modMpuDt.gyroVar > VARIATION_GYRO_THRESHOLD)
    {
      Serial.printf("___________Gia toc lon: %.2f \r\n", modMpuDt.accelVar);
      mainDt.state = VARIATION_DETECT;
      startCounter = millis();
    }
    break;
  case VARIATION_DETECT:
  {
    unsigned long currentTime = millis();
    if (currentTime - startCounter > WAIT_HEARBEAT_TIMEOUT)
    {
      Serial.println("_______________Het thoi gian");
      mainDt.state = IDLE;
      break;
    }
    if (modPoxDt.avgHeartRate > VARIATION_HEARBEAT_THRESHOLD)
    {
      Serial.printf("_______________Nhip tim lon: %.2f \r\n", modPoxDt.avgHeartRate);
      mainDt.state = HERTBEAT_DETECT;
      startCounter = millis();
    }
    break;
  }
  case HERTBEAT_DETECT:
    mainDt.state = FALL_DETECT_ALARM;
    startRinging();
    mqttPublish(true);
    Serial.println("hungdan bieu print");
    // if (currentTime - startCounter > WAIT_HEARBEAT_TIMEOUT)
    // {
    //   mainDt.state = IDLE;
    //   break;
    // }
    break;
  case FALL_DETECT_ALARM:
    // Serial.printf("hungdan bieu print %.2f %.2f\r\n", modMpuDt.accelVar, modPoxDt.avgHeartRate);
    if (modGpioDt.pinStopAlarmPressed == true)
    {
      Serial.println("hungdan bieu clear");
      modGpioDt.pinStopAlarmPressed = false;
      turn_off_buzzer();
      Reset();
      mqttPublish(false);
      modPoxDt.avgHeartRate = 0;
      modMpuDt.accelVar = 0;
      mainDt.state = IDLE;
    }
    break;
  default:
    break;
  }

  if (millis() - mainDt.fingerCheckStartTime > POX_FINGER_TIMEOUT)
  {
    mainDt.fingerCheckStartTime = millis();
    // Nếu không phát hiện ngón tay trong thời gian quy định, vào chế độ tiết kiệm năng lượng
    _enterLightSleepMode();
  }
}

static void _enterLightSleepMode()
{
  mqttPublish(false);
  turn_off_buzzer();

  // Cấu hình nguồn wake-up bằng GPIO
  MOD_POX_Shutdown();

  esp_sleep_enable_gpio_wakeup();

  // Thiết lập GPIO5 làm nguồn đánh thức khi mức LOW
  gpio_wakeup_enable(PIN_WAKEUP, GPIO_INTR_LOW_LEVEL);

  // In thông báo trước khi ngủ
  Serial.println("Entering light-sleep mode.");

  // Tắt OLED display
  MOD_OLED_Shutdown();

  // Flush UART trước khi ngủ
  Serial.flush();

  // Vào chế độ light-sleep
  esp_light_sleep_start();

  // Tiếp tục thực thi sau khi đánh thức
  Serial.println("Wake up from light-sleep.");
  mainDt.fingerCheckStartTime = millis();

  // Khởi động lại các thiết bị ngoại vi

  MQTT_Init();
  MOD_GPIO_Init();
  MOD_MPU_Init();
  MOD_OLED_Init();
  MOD_POX_Init();
  MOD_BUZZER_Init();
  mainDt.state = IDLE;
}
