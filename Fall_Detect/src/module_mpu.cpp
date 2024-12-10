#include "module_mpu.h"

MPU6050 mpu;
mod_mpu_data modMpuDt;

float accelBuffer[BUFFER_SIZE];
uint8_t accelBuffIdx;
uint16_t accelCurrentBuffSz;
float gyroBuffer[BUFFER_SIZE];
uint8_t gyroBuffIdx;
uint16_t gyroCurrentBuffSz;

static float _calNewAccelAvg(float newVal, float avg);
static float _calNewGyroAvg(float newVal, float avg);
static float _calculateMaxChange(float measurements[], uint8_t numMeasurements);
static float _calculateMinChange(float measurements[], uint8_t numMeasurements);

void MOD_MPU_Init()
{
    // Khởi tạo MPU6050
    Serial.println("MPU6050: Initializing ...");
    mpu.initialize();
    if (!mpu.testConnection())
    {
        Serial.println("MPU6050: Init FAILED!");
        while (1)
            ;
    }
    Serial.println("MPU6050: Init SUCCESS!");
    memset(accelBuffer, 0, sizeof(accelBuffer));
    accelCurrentBuffSz = 0;
    accelBuffIdx = 0;
    memset(gyroBuffer, 0, sizeof(gyroBuffer));
    gyroBuffIdx = 0;
    gyroCurrentBuffSz = 0;
    modMpuDt.state = MPU_MEASURING;
    modMpuDt.accelVar = 0;
    modMpuDt.gyroVar = 0;
}

void MOD_MPU_Task()
{
    switch (modMpuDt.state)
    {
    case MPU_IDLE:
        /* code */
        break;
    case MPU_MEASURING:
    {
        int16_t ax, ay, az;
        int16_t gx, gy, gz;

        mpu.getAcceleration(&ax, &ay, &az);
        mpu.getRotation(&gx, &gy, &gz);

        // Tính độ lớn vector gia tốc
        float newAccel = sqrt(((float)ax / 4096) * ((float)ax / 4096) + ((float)ay / 4096) * ((float)ay / 4096) + ((float)az / 4096) * ((float)az / 4096));
        // Tính độ lớn vector con quay
        float newGyro = sqrt(((float)gx / 65.5) * ((float)gx / 65.5) + ((float)gy / 65.5) * ((float)gy / 65.5) + ((float)gz / 65.5) * ((float)gz / 65.5));
        // float newAccel = sqrt(((float)ax / 1) * ((float)ax / 1) + ((float)ay / 1) * ((float)ay / 1) + ((float)az / 1) * ((float)az / 1));
        // float newGyro = sqrt(((float)gx / 1) * ((float)gx / 1) + ((float)gy / 1) * ((float)gy / 1) + ((float)gz / 1) * ((float)gz / 1));

        // modMpuDt.accelAvg = _calNewAccelAvg(newAccel, modMpuDt.accelAvg);
        // modMpuDt.gyroAvg = _calNewGyroAvg(newGyro, modMpuDt.gyroAvg);

        accelBuffer[accelBuffIdx] = newAccel;
        gyroBuffer[gyroBuffIdx] = newGyro;
        float newMaxAccel = _calculateMaxChange(accelBuffer, accelBuffIdx);
        float newMinAccel = _calculateMinChange(accelBuffer, accelBuffIdx);
        modMpuDt.accelVar = abs(newMaxAccel - newMinAccel);
        accelBuffIdx = (accelBuffIdx + 1) % BUFFER_SIZE;
        float newMaxGyro = _calculateMaxChange(gyroBuffer, gyroBuffIdx);
        float newMinGyro = _calculateMinChange(gyroBuffer, gyroBuffIdx);
        modMpuDt.gyroVar = abs(newMaxGyro - newMinGyro);
        gyroBuffIdx = (gyroBuffIdx + 1) % BUFFER_SIZE;

        // Serial.printf("Bien thien: %.2f %.2f \r\n", accelVariation, gyroVariation);
        // if (accelBuffIdx == BUFFER_SIZE - 1)
        // {
        //     Serial.print("Độ lớn gia tốc: ");
        //     Serial.println(modMpuDt.accelAvg);
        // }
        // if (gyroBuffIdx == BUFFER_SIZE - 1)
        // {
        //     Serial.print("Độ lớn con quay: ");
        //     Serial.println(modMpuDt.gyroAvg);
        // }
        break;
    }
    default:
        break;
    }
}

// Hàm tính biến thiên lớn nhất trong mảng
static float _calculateMaxChange(float measurements[], uint8_t numMeasurements)
{
    float maxChange = 0.0;
    for (uint8_t i = 1; i < numMeasurements; i++)
    {
        float change = fabs(measurements[i] - measurements[i - 1]);
        if (change > maxChange)
        {
            maxChange = change;
        }
    }
    return maxChange;
}

// Hàm tính biến thiên nhỏ nhất trong mảng
static float _calculateMinChange(float measurements[], uint8_t numMeasurements)
{
    float minChange = fabs(measurements[1] - measurements[0]);
    for (uint8_t i = 1; i < numMeasurements; i++)
    {
        float change = fabs(measurements[i] - measurements[i - 1]);
        if (change < minChange)
        {
            minChange = change;
        }
    }
    return minChange;
}

static float _calNewAccelAvg(float newVal, float avg)
{
    float oldSum = avg * accelCurrentBuffSz;
    float newSum = oldSum - accelBuffer[accelBuffIdx] + newVal;
    accelBuffer[accelBuffIdx] = newVal;

    accelCurrentBuffSz += (accelCurrentBuffSz < BUFFER_SIZE) ? 1 : 0;
    avg = newSum / accelCurrentBuffSz;

    accelBuffIdx++;
    if (accelBuffIdx >= BUFFER_SIZE)
    {
        accelBuffIdx = 0; // Quay lại đầu buffer
    }
    return avg;
}
static float _calNewGyroAvg(float newVal, float avg)
{
    float oldSum = avg * gyroCurrentBuffSz;
    float newSum = oldSum - gyroBuffer[gyroBuffIdx] + newVal;
    gyroBuffer[gyroBuffIdx] = newVal;

    gyroCurrentBuffSz += (gyroCurrentBuffSz < BUFFER_SIZE) ? 1 : 0;
    avg = newSum / gyroCurrentBuffSz;

    gyroBuffIdx++;
    if (gyroBuffIdx >= BUFFER_SIZE)
    {
        gyroBuffIdx = 0; // Quay lại đầu buffer
    }
    return avg;
}