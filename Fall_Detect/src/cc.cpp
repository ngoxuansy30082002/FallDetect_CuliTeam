#include "cc.h"
// #include <eloquent_tinyml.h>   // Library for machine learning functionalities
#include "model.h" // Header file for trained machine learning model
#include <WiFi.h>  // WiFi library for connectivity

#define NUMBER_OF_INPUTS 22 // Number of inputs expected by the ML model
#define NUMBER_OF_OUTPUTS 2 // Number of outputs from the ML model

// WiFi credentials
const char ssid[] = "";
const char pass[] = "";

// Define pins for auxiliary devices
const int ADD0 = 5; // I2C address select pin
const int BUZ = 10; // Buzzer pin

bool fallDetected = false; // Boolean flag for fall detection status

// Acceleration thresholds for detecting a fall
const float ACCEL_THRESHOLD_HIGH = 15.0;
const float ACCEL_THRESHOLD_LOW = 2.0;

// Complementary filter constant for sensor fusion
const float alpha = 0.98;
float filteredPitch = 0.0; // Filtered pitch angle
float filteredRoll = 0.0;  // Filtered roll angle

// Define MPU9250 sensor objects
MPU6050 mpucc;
// Structure to hold sensor data
struct SensorData
{
    unsigned long timestamp;   // Timestamp of the sensor reading
    float aX, aY, aZ;          // Accelerometer readings
    float gX, gY, gZ;          // Gyroscope readings
    float mX, mY, mZ;          // Magnetometer readings
    float pitch, roll;         // Orientation angles
    float prevPitch, prevRoll; // Previous orientation angles
};

// Instances of sensor data for two MPU9250 sensors
SensorData sensorData1;

// Structure to hold calibration offsets
struct CalibrationOffsets
{
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    float magX, magY, magZ;
};

// Instances of calibration offsets for each sensor
CalibrationOffsets calibrationOffsets1;

// Machine learning classifier object
Eloquent::ML::Port::SVM classifier;

// Function to calibrate sensors
void calibrateSensors(MPU6050 mpu, CalibrationOffsets &calibrationOffsets)
{
    Serial.println("Calibrating sensors...");
    int numSamples = 600; // Number of samples for calibration
    float sumAccelX = 0, sumAccelY = 0, sumAccelZ = 0;
    float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;

    for (int i = 0; i < numSamples; i++)
    {
        int16_t ax, ay, az;
        int16_t gx, gy, gz;

        mpu.getAcceleration(&ax, &ay, &az);
        mpu.getRotation(&gx, &gy, &gz);
        // Accumulate sensor readings for calibration
        sumAccelX += ax;
        sumAccelY += ay;
        sumAccelZ += az;
        sumGyroX += gx;
        sumGyroY += gy;
        sumGyroZ += gz;

        delay(20); // Delay to allow sensor settling
    }

    // Calculate average offsets
    calibrationOffsets.accelX = sumAccelX / numSamples;
    calibrationOffsets.accelY = sumAccelY / numSamples;
    calibrationOffsets.accelZ = sumAccelZ / numSamples;
    calibrationOffsets.gyroX = sumGyroX / numSamples;
    calibrationOffsets.gyroY = sumGyroY / numSamples;
    calibrationOffsets.gyroZ = sumGyroZ / numSamples;

    Serial.println("Calibration complete.");
}

// Function to read and process sensor data
void readSensorData(MPU6050 mpu, SensorData &sensorData, CalibrationOffsets &calibrationOffsets)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);
    // Update and apply offsets to sensor readings
    sensorData.aX = ax - calibrationOffsets.accelX;
    sensorData.aY = ay - calibrationOffsets.accelY;
    sensorData.aZ = az - calibrationOffsets.accelZ;

    sensorData.gX = gx - calibrationOffsets.gyroX;
    sensorData.gY = gy - calibrationOffsets.gyroY;
    sensorData.gZ = gz - calibrationOffsets.gyroZ;

    // Time calculation for integration
    static unsigned long lastTime = millis();
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0; // Convert to seconds
    lastTime = currentTime;

    // Calculate pitch and roll using accelerometer data
    float accelPitch = atan2(sensorData.aY, sqrt(pow(sensorData.aX, 2) + pow(sensorData.aZ, 2))) * 180.0 / PI;
    float accelRoll = atan2(-sensorData.aX, sensorData.aZ) * 180.0 / PI;

    // Integrate gyroscope data
    float gyroPitch = sensorData.gX * dt;
    float gyroRoll = sensorData.gY * dt;

    // Apply complementary filter for sensor fusion
    filteredPitch = alpha * (filteredPitch + gyroPitch) + (1.0 - alpha) * accelPitch;
    filteredRoll = alpha * (filteredRoll + gyroRoll) + (1.0 - alpha) * accelRoll;

    // Store filtered values in sensor data structure
    sensorData.pitch = filteredPitch;
    sensorData.roll = filteredRoll;
}

void cc_init()
{
   

    // Initialize sensors
    Serial.println("MPU6050: Initializing ...");
    mpucc.initialize();
    if (!mpucc.testConnection())
    {
        Serial.println("MPU6050: Init FAILED!");
        while (1)
            ;
    }

    // Calibrate sensors
    calibrateSensors(mpucc, calibrationOffsets1);

    // Read initial sensor data
    readSensorData(mpucc, sensorData1, calibrationOffsets1);

    // Buzzer initialization to indicate startup
}

void cc_task()
{
    // Continuously read sensor data
    readSensorData(mpucc, sensorData1, calibrationOffsets1);

    // Calculate total acceleration for fall detection
    float totalAccel1 = sqrt(sensorData1.aX * sensorData1.aX + sensorData1.aY * sensorData1.aY + sensorData1.aZ * sensorData1.aZ);

    // Perform prediction only if acceleration exceeds threshold
    if (totalAccel1 > ACCEL_THRESHOLD_HIGH)
    {
        // Prepare features array for prediction
        float features[NUMBER_OF_INPUTS] = {
            // Sensor data from MPU1
            sensorData1.aX,
            sensorData1.aY,
            sensorData1.aZ,
            sensorData1.gX,
            sensorData1.gY,
            sensorData1.gZ,
            sensorData1.mX,
            sensorData1.mY,
            sensorData1.mZ,
            sensorData1.pitch,
            sensorData1.roll,
            // Sensor data from MPU2
            sensorData1.aX,
            sensorData1.aY,
            sensorData1.aZ,
            sensorData1.gX,
            sensorData1.gY,
            sensorData1.gZ,
            sensorData1.mX,
            sensorData1.mY,
            sensorData1.mZ,
            sensorData1.pitch,
            sensorData1.roll,
        };

        unsigned long startTime = millis(); // Start time for computation timing

        // Perform prediction using the trained ML model
        int result = classifier.predict(features);

        unsigned long endTime = millis();                    // End time for computation timing
        unsigned long computationTime = endTime - startTime; // Calculate computation time

        // Output prediction result and computation time
        Serial.print("Prediksi kelas: ");
        Serial.println(result);
        Serial.print("Waktu komputasi: ");
        Serial.print(computationTime);
        Serial.println(" ms");

        // Execute actions based on prediction result
        if (result == 1)
        { // Fall detected
            Serial.println("Fall detected!");
            fallDetected = true;  // Set fall detected flag
            digitalWrite(BUZ, 1); // Activate buzzer
            delay(2000);          // Keep buzzer on for 2 seconds
            digitalWrite(BUZ, 0); // Turn off buzzer
        }
        else
        {
            fallDetected = false; // Reset fall detected flag
            digitalWrite(BUZ, 0); // Ensure buzzer is off if no fall
        }
    }
}