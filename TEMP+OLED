#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2                // DS18B20 data wire is connected to input 2
#define OLED_RESET 4                  // Adafruit needs this but we don't use for I2C

DeviceAddress thermometerAddress;     // custom array type to hold 64 bit device address

Adafruit_SSD1306 display(OLED_RESET); // create a display instance
OneWire oneWire(ONE_WIRE_BUS);        // create a oneWire instance to communicate with temperature IC
DallasTemperature tempSensor(&oneWire);  // pass the oneWire reference to Dallas Temperature


void setup()   {

  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C address of the display
  display.clearDisplay();                     // clear the display buffer
  display.display();                          // update display

  Serial.println("DS18B20 Temperature IC Test");
  Serial.println("Locating devices...");
  tempSensor.begin();                         // initialize the temp sensor

  if (!tempSensor.getAddress(thermometerAddress, 0))
    Serial.println("Unable to find Device.");
  else {
    Serial.print("Device 0 Address: ");
    printAddress(thermometerAddress);
    Serial.println();
  }

  tempSensor.setResolution(thermometerAddress, 11);      // set the temperature resolution (9-12)
}


void loop() {

  tempSensor.requestTemperatures();                      // request temperature sample from sensor on the one wire bus
  displayTemp(tempSensor.getTempC(thermometerAddress));  // show temperature on OLED display

  delay(500);                                            // update readings every half a second
}

void displayTemp(float temperatureReading) {             // temperature comes in as a float with 2 decimal places

  // set up OLED text size and print the temperature data
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temp:");
  display.print(DallasTemperature::toFahrenheit(temperatureReading), 2); // rounded to 1 decimal place
  display.print((char)247);                                              // degree symbol
  display.println("F");
  Serial.print(DallasTemperature::toFahrenheit(temperatureReading));     // serial debug output
  Serial.print("°");
  Serial.println("F");

  display.display();                    // update the OLED display with all the new text
}


// print device address from the address array
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
