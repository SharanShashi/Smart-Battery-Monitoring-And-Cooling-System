#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Define DHT sensor and type
#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Define OLED display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define pins for LEDs, fan, and buzzer
#define RED_LED_PIN D5
#define GREEN_LED_PIN D6
#define FAN_PIN D7
#define BUZZER_PIN D8

// Define pin for battery voltage reading
#define BATTERY_PIN A0

// Define temperature threshold and battery voltage limit
const float TEMP_THRESHOLD = 30.0;  // Adjust as needed
const float MAX_BATTERY_VOLTAGE = 4.2;

// Helper function to read battery voltage
float readBatteryVoltage() {
  int sensorValue = analogRead(BATTERY_PIN); // Read the analog value
  float voltage = sensorValue * (5.0 / 1023.0) * 2; // Convert to voltage (assuming a voltage divider)
  return voltage;
}

void setup() {
  Serial.begin(115200); // Initialize serial communication
  dht.begin(); // Initialize DHT sensor

  // Set pin modes for LEDs, fan, and buzzer
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Stop if OLED display allocation fails
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
}

void loop() {
  // Read temperature and humidity from DHT sensor
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  float batteryVoltage = readBatteryVoltage(); // Read battery voltage
  float batteryPercentage = (batteryVoltage / MAX_BATTERY_VOLTAGE) * 100; // Calculate battery percentage

  // Check if temperature crosses threshold
  if (temp > TEMP_THRESHOLD) {
    digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
    digitalWrite(FAN_PIN, HIGH); // Turn on cooling fan
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
    digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
  } else {
    digitalWrite(FAN_PIN, LOW); // Turn off cooling fan
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    if (batteryVoltage <= MAX_BATTERY_VOLTAGE) {
      digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED if battery voltage is safe
    } else {
      digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED if battery voltage is high
    }
  }

  // Check if battery voltage exceeds limit
  if (batteryVoltage > MAX_BATTERY_VOLTAGE) {
    digitalWrite(RED_LED_PIN, HIGH); // Turn on red LED
    digitalWrite(GREEN_LED_PIN, LOW); // Turn off green LED
  } else {
    digitalWrite(RED_LED_PIN, LOW); // Turn off red LED if battery voltage is safe
    if (temp <= TEMP_THRESHOLD) {
      digitalWrite(GREEN_LED_PIN, HIGH); // Turn on green LED if temperature is safe
    }
  }

  // Display readings on OLED with increased line spacing
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temp);
  display.println(" C");

  display.setCursor(0, 16); // Increased spacing
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.setCursor(0, 32); // Increased spacing
  display.print("Voltage: ");
  display.print(batteryVoltage);
  display.println(" V");

  display.setCursor(0, 48); // Increased spacing
  display.print("Battery: ");
  display.print(batteryPercentage);
  display.println(" %");

  display.display();

  delay(2000);  // Adjust delay as needed
}
