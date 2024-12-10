#include "module_oled.h"

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Không có chân reset
mod_oled_data modOledDt;

static void _oledDiplayWaitFinger();
static void _oledDisplayMeasuring();

void MOD_OLED_Init()
{
    // Khởi tạo OLED display với địa chỉ I2C 0x3C
    Serial.println("POX: Initializing ... ");

    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println("OLED: Init FAILED!");
        while (1)
            ;
    }
    Serial.println("OLED: Init SUCESS!");

    delay(1000);              // Chờ OLED khởi tạo
    oled.clearDisplay();      // Xóa màn hình
    oled.setTextSize(1);      // Đặt kích thước chữ
    oled.setTextColor(WHITE); // Đặt màu chữ
    modOledDt.state = OLED_DISPLAY_WAIT_FINGER;
    modOledDt.sleepFlag = false;
}

void MOD_OLED_Task()
{
    if (modGpioDt.pinOledPressed)
    {
        modGpioDt.pinOledPressed = false;
        if (modOledDt.sleepFlag)
        {
            MOD_OLED_Shutdown();
            return;
        }
        oled.ssd1306_command(SSD1306_DISPLAYON);
    }

    switch (modOledDt.state)
    {
    case OLED_IDLE:
    {
        break;
    }
    case OLED_POWER_OFF:
    {
        break;
    }
    case OLED_DISPLAY_WAIT_FINGER:
    {
        // _oledDiplayWaitFinger();
        _oledDisplayMeasuring();

        break;
    }
    case OLED_DISPLAY_MEASURING:
    {
        _oledDisplayMeasuring();
        break;
    }
    }
}
void MOD_OLED_EnterState(OLED_display_state_t state)
{
    modOledDt.state = state;
}

void MOD_OLED_Shutdown()
{
    oled.ssd1306_command(SSD1306_DISPLAYOFF);
    modOledDt.state = OLED_POWER_OFF;
}

static void _oledDiplayWaitFinger()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("Please place finger");
    oled.print("Time left: ");
    oled.print((POX_FINGER_TIMEOUT - (millis() - mainDt.fingerCheckStartTime)) / 1000);
    oled.println(" s");
    oled.display();
}

static void _oledDisplayMeasuring()
{
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print("Heart rate: ");
    oled.print(modPoxDt.heartRate);
    oled.println(" bpm");

    oled.setCursor(0, 8);
    oled.print("SpO2: ");
    oled.print(modPoxDt.SpO2);
    oled.println(" %");

    oled.setCursor(0, 16);
    // Hiển thị giá trị trung bình mới nhất
    oled.print("Avg HR: ");
    oled.print(modPoxDt.avgHeartRate);
    oled.println(" bpm");

    // oled.print("Battery: ");
    // oled.print(in_voltage);
    // oled.println(" %");

    oled.setCursor(0, 32);
    // Hiển thị độ lớn gia tốc
    oled.print("Accel Mag: ");
    oled.print(modMpuDt.accelVar, 2);
    oled.println();

    if (modOledDt.state == OLED_DISPLAY_WAIT_FINGER)
    {
        oled.setCursor(0, 48);
        oled.println("Sleeping after: ");
        oled.print((POX_FINGER_TIMEOUT - (millis() - mainDt.fingerCheckStartTime)) / 1000);
        oled.println(" s");
    }
    // oled.setCursor(0, 48);
    // oled.print("Time left: ");
    // oled.print(countdown);
    // oled.println(" s");

    oled.display();
}