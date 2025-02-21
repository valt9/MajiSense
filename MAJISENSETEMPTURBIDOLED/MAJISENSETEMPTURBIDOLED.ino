#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2                // DS18B20 data wire is connected to pin 2
#define OLED_RESET 4                  // Required for Adafruit display library

DeviceAddress thermometerAddress;     // Store the 64-bit device address of DS18B20

Adafruit_SSD1306 display(OLED_RESET); // Create display instance
OneWire oneWire(ONE_WIRE_BUS);        // Create a oneWire instance
DallasTemperature tempSensor(&oneWire);  // Create a DallasTemperature instance

// Calibration values for Turbidity Sensor
#define V_CLEAR 4.2  // Voltage in clear water (adjust based on real readings)
#define V_TURBID 2.5 // Voltage in turbid water (adjust based on real readings)

void setup() {
  Serial.begin(9600);

  // Initialize OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  // Initialize Temperature Sensor
  Serial.println("Initializing DS18B20 Temperature Sensor...");
  tempSensor.begin();

  if (!tempSensor.getAddress(thermometerAddress, 0)) {
    Serial.println("Unable to find DS18B20 Device.");
  } else {
    Serial.print("Device 0 Address: ");
    printAddress(thermometerAddress);
    Serial.println();
  }

  tempSensor.setResolution(thermometerAddress, 11);  // Set temperature resolution (9-12 bits)
}

void loop() {
  // Read temperature
  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempC(thermometerAddress);
  float tempF = DallasTemperature::toFahrenheit(tempC); // Convert to Fahrenheit

  // Read turbidity sensor
  int sensorValue = analogRead(A0); // Read from analog pin A0
  float voltage = sensorValue * (5.0 / 1024.0); // Convert ADC value to voltage

  // Convert voltage to NTU (0 = Clear, 250 = Turbid)
  float NTU = ((V_CLEAR - voltage) / (V_CLEAR - V_TURBID)) * 250;
  
  // Ensure NTU stays within range
  if (NTU > 250) NTU = 250;
  if (NTU < 0) NTU = 0;

  // Display temperature and turbidity (NTU)
  displayData(tempF, NTU, voltage);

  delay(500);  // Update readings every 500ms
}

void displayData(float temperature, float NTU, float voltage) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // Display temperature
  display.println("Temp:");
  display.print(temperature, 2);
  display.print((char)247); // Degree symbol
  display.println("F");

  // Display NTU (Turbidity)
  display.println("Turbidity:");
  display.print(NTU, 2);
  display.println(" NTU");

  // Serial Monitor Output (for debugging)
  Serial.print("Temp: ");
  Serial.print(temperature, 2);
  Serial.println(" F");

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  Serial.print("NTU: ");
  Serial.print(NTU, 2);
  Serial.println(" NTU");

  display.display();  // Update OLED display
}

// Print DS18B20 sensor address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

