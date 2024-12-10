#include "module_pox.h"

PulseOximeter pox;
mod_pox_data modPoxDt;

static void _onBeatDetected();
static void _addToSlidingWindow(float heartRate, float SpO2);
static float _getSlidingAverage(float *buffer, int count);

static float heartRateWindow[WINDOW_SIZE];
static float SpO2Window[WINDOW_SIZE];
static int windowIndex = 0;
static int windowCount = 0;
static unsigned long tsLastReport;
static bool _fingerDetectCallback;

void MOD_POX_Init()
{
    Serial.println("POX: Initializing ... ");
    if (!pox.begin())
    {
        Serial.println("POX: Init FAILED!");
        while (1)
            ;
    }
    else
    {
        Serial.println("POX: Init SUCCESS!");
    }
    pox.setOnBeatDetectedCallback(_onBeatDetected);

    modPoxDt.state = POX_IDLE;
    memset(heartRateWindow, 0, sizeof(heartRateWindow));
    memset(SpO2Window, 0, sizeof(SpO2Window));
    windowIndex = 0;
    windowCount = 0;
}

void MOD_POX_Task()
{
    // Cập nhật dữ liệu từ pulse oximeter
    pox.update();

    switch (modPoxDt.state)
    {
    case POX_IDLE:
    {
        if (millis() - tsLastReport > REPORTING_PERIOD_MS)
        {
            tsLastReport = millis();
            modPoxDt.state = POX_MEASURING;
        }
        break;
    }
    case POX_MEASURING:
    {
        // Lấy giá trị nhịp tim và SpO2
        float heartRate = pox.getHeartRate();
        float SpO2 = pox.getSpO2();

        // Serial.printf("%.2f %.2f\r\n", heartRate, SpO2);

        bool _fingerDetectRecipe = !isnan(heartRate) && heartRate > 0 && !isnan(SpO2) && SpO2 > 0;

        modPoxDt.fingerDetected = (_fingerDetectRecipe || _fingerDetectCallback);
        if (modPoxDt.fingerDetected == false)
        {
            MOD_OLED_EnterState(OLED_DISPLAY_WAIT_FINGER);
            modPoxDt.state = POX_IDLE;
            return;
        }
        _fingerDetectCallback = false;

        mainDt.fingerCheckStartTime = millis();
        modPoxDt.heartRate = heartRate;
        modPoxDt.SpO2 = SpO2;

        // Add new readings to the sliding window
        _addToSlidingWindow(heartRate, SpO2);

        // Calculate sliding averages
        modPoxDt.avgHeartRate = _getSlidingAverage(heartRateWindow, windowCount);
        modPoxDt.avgSpO2 = _getSlidingAverage(SpO2Window, windowCount);

        MOD_OLED_EnterState(OLED_DISPLAY_MEASURING);
        modPoxDt.state = POX_IDLE;
        break;
    }
    case POX_POWER_OFF:
        break;
    }
}

void Reset()
{
    memset(heartRateWindow, 0, sizeof(heartRateWindow));
    memset(SpO2Window, 0, sizeof(SpO2Window));
    windowIndex = 0;
    windowCount = 0;
}
void MOD_POX_Shutdown()
{
    modPoxDt.state = POX_POWER_OFF;
    pox.shutdown();
}

static void _onBeatDetected()
{
    // Serial.println("beat");
    _fingerDetectCallback = true;
}

static void _addToSlidingWindow(float heartRate, float SpO2)
{
    heartRateWindow[windowIndex] = heartRate;
    SpO2Window[windowIndex] = SpO2;
    windowIndex = (windowIndex + 1) % WINDOW_SIZE;
    if (windowCount < WINDOW_SIZE)
    {
        windowCount++;
    }
}

// Function to calculate the average from the sliding window
static float _getSlidingAverage(float *buffer, int count)
{
    float sum = 0;
    for (int i = 0; i < count; i++)
    {
        sum += buffer[i];
    }
    return (count > 0) ? (sum / count) : 0;
}
