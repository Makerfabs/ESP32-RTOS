#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

// define two tasks
void Task1(void *pvParameters);
void Task2(void *pvParameters);

void setup()
{
    Serial.begin(115200);

    xTaskCreatePinnedToCore(Task1, "Task1", 1024, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task2, "Task2", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task3, "Task3", 1024, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
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

    int index = 0;
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

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Test 2"));
    display.display();

    delay(1000);

    while (1) // A Task shall never return or exit.
    {
        display.clearDisplay();
        display.setCursor(0, 40); // Start at top-left corner
        display.println(index++);
        display.display();
        vTaskDelay(1000);
    }
}

void Task2(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    pinMode(PIN_B, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_B, HIGH);
        vTaskDelay(10);
        digitalWrite(PIN_B, LOW);
        vTaskDelay(10);
    }
}

void Task3(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    ledcAttachPin(PIN_C, 1); // assign RGB led pins to channels
    ledcSetup(1, 1000, 8);   // 12 kHz PWM, 8-bit resolution

    while (1) // A Task shall never return or exit.
    {
        for (int i = 0; i < 256; i += 64)
        {
            ledcWrite(1, i);
            vTaskDelay(4);
        }
        for (int i = 255; i >= 0; i -= 64)
        {
            ledcWrite(1, i);
            vTaskDelay(4);
        }
    }
}
