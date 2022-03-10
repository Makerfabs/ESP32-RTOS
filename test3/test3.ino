#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Simple_HCSR04.h>


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define PIN_A 15
#define PIN_B 0
#define PIN_C 25

//SSD1306
#define MAKEPYTHON_ESP32_SDA 4
#define MAKEPYTHON_ESP32_SCL 5
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//DHT10
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//HC SR-04
#define ECHO_PIN 26 /// the pin at which the sensor echo is connected
#define TRIG_PIN 27 /// the pin at which the sensor trig is connected
Simple_HCSR04 *sensor;

//Global value
int count = 0;
int humidity_value = 0;
int temperature_value = 0;
unsigned long distance = 0;

void setup()
{
    Serial.begin(115200);

    xTaskCreatePinnedToCore(Task1, "Task1", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task2, "Task2", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task3, "Task3", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task4, "Task4", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
}

void loop()
{
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void Task1(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    //LCD
    Wire.begin(MAKEPYTHON_ESP32_SDA, MAKEPYTHON_ESP32_SCL);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();

    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Test 2"));
    display.display();

    delay(1000);

    while (1) // A Task shall never return or exit.
    {
        display.clearDisplay();
        display.setCursor(0, 0); // Start at top-left corner
        display.println((String) "Count:" + count);
        display.setCursor(0, 10);
        display.println((String) "Hum:" + humidity_value + " %");
        display.setCursor(0, 20);
        display.println((String) "Tem:" + temperature_value + " C");
        display.setCursor(0, 30);
        display.println((String) "Dis:" + distance + " cm");
        display.display();
        vTaskDelay(200);
    }
}

void Task2(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    while (1) // A Task shall never return or exit.
    {
        count++;
        vTaskDelay(1000);
    }
}

void Task3(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    dht.begin();

    while (1) // A Task shall never return or exit.
    {
        // Serial.println("Read dht");
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();

        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t))
        {
            Serial.println(F("Failed to read from DHT sensor!"));
        }

        else
        {
            // Serial.print(F("Humidity: "));
            // Serial.print(h);
            // Serial.print(F("%  Temperature: "));
            // Serial.print(t);
            // Serial.print(F("°C "));

            humidity_value = (int)h;
            temperature_value = (int)t;
        }

        vTaskDelay(3000);
    }
}

void Task4(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    sensor = new Simple_HCSR04(ECHO_PIN, TRIG_PIN);

    while (1) // A Task shall never return or exit.
    {
        distance = sensor->measure()->cm();
        // Serial.print("distance: ");
        // Serial.print(distance);
        // Serial.print("cm\n");

        vTaskDelay(200);
    }
}
