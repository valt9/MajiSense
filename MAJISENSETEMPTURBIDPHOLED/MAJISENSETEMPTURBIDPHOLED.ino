#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temperature Sensor (DS18B20)
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
DeviceAddress thermometerAddress;

// pH Sensor
#define PH_PIN A1
float voltage, pH_value;
const float calibration_value = 3.3;  // Adjust for calibration

// Turbidity Sensor
#define TURBIDITY_PIN A0
#define V_CLEAR 4.2   // Voltage in clear water (adjust based on calibration)
#define V_TURBID 2.5  // Voltage in turbid water (adjust based on calibration)

void setup() {
    Serial.begin(9600);

    // Initialize OLED Display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true);  // Stop execution if display fails
    }
    display.clearDisplay();
    display.display();

    // Initialize Temperature Sensor
    tempSensor.begin();
    if (!tempSensor.getAddress(thermometerAddress, 0)) {
        Serial.println("Unable to find DS18B20 Device.");
    } else {
        Serial.print("Device Address: ");
        printAddress(thermometerAddress);
        Serial.println();
    }
    tempSensor.setResolution(thermometerAddress, 11);
}

void loop() {
    // Read Temperature
    tempSensor.requestTemperatures();
    float tempC = tempSensor.getTempC(thermometerAddress);
    float tempF = DallasTemperature::toFahrenheit(tempC);

    // Read Turbidity Sensor
    int turbidityValue = analogRead(TURBIDITY_PIN);
    float turbidityVoltage = turbidityValue * (5.0 / 1023.0);
    float NTU = ((V_CLEAR - turbidityVoltage) / (V_CLEAR - V_TURBID)) * 250;
    NTU = constrain(NTU, 0, 250);

    // Read pH Sensor
    int pH_SensorValue = analogRead(PH_PIN);
    voltage = pH_SensorValue * (5.0 / 1023.0);
    pH_value = 7 + ((calibration_value - voltage) * 3.5);

    // Clear OLED display and update with new values
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.println("Water Quality Monitor");

    display.setCursor(0, 15);
    display.print("Temp: "); display.print(tempF, 1); display.println(" F");

    display.setCursor(0, 30);
    display.print("Turbidity: "); display.print(NTU, 1); display.println(" NTU");

    display.setCursor(0, 45);
    display.print("pH: "); display.print(pH_value, 2);

    display.display();  // Refresh display

    // Serial Output for Debugging
    Serial.print("Temp: "); Serial.print(tempF, 1); Serial.println(" F");
    Serial.print("Turbidity: "); Serial.print(NTU, 1); Serial.println(" NTU");
    Serial.print("pH: "); Serial.print(pH_value, 2); Serial.println();

    delay(1000);  // Refresh every second
}

// Function to print DS18B20 sensor address
void printAddress(DeviceAddress deviceAddress) {
    for (uint8_t i = 0; i < 8; i++) {
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}