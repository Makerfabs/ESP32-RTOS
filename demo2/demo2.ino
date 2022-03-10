#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include <SPI.h>
#include <LovyanGFX.hpp>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>

#include "makerfabs_pin.h"

#define COLOR_BACKGROUND 0xEF9E
#define COLOR_BUTTON TFT_WHITE
#define COLOR_TEXT 0x322B
#define COLOR_LINE TFT_BLACK
#define COLOR_SHADOW 0x4BAF

//Sensor
#define DHTPIN 18     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SGP30 sgp;
#define I2C_SDA 26
#define I2C_SCL 27

#define BUZZPIN 5

//TFT
struct LGFX_Config
{
    static constexpr spi_host_device_t spi_host = VSPI_HOST;
    static constexpr int dma_channel = 1;
    static constexpr int spi_sclk = LCD_SCK;
    static constexpr int spi_mosi = LCD_MOSI;
    static constexpr int spi_miso = LCD_MISO;
};

static lgfx::LGFX_SPI<LGFX_Config> tft;
static LGFX_Sprite sprite(&tft);
static lgfx::Panel_ILI9488 panel;

//Global value
int humidity_value = 0;
int temperature_value = 0;
int tvoc_value = 0;
int co2_value = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LCD_CS, OUTPUT);

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    Wire.begin(I2C_SDA, I2C_SCL);

    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_DHT11, "Task_DHT11", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_SGP30, "Task_SGP30", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_BUZZ, "Task_BUZZ", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop()
{
}

void Task_TFT(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    tft_init();
    page_title("FreeRTOS Demo 2");

    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(2);

    while (1) // A Task shall never return or exit.
    {
        //Display

        tft.fillRect(0, 80, 200, 80, COLOR_BACKGROUND);
        tft.setCursor(0, 80);

        tft.println((String) "HUMI:" + humidity_value + " %");
        tft.println((String) "TEMP:" + temperature_value) + " C";
        tft.println((String) "TVOC:" + tvoc_value) + " ppb";
        tft.println((String) "ECO2 :" + co2_value) + " ppm";
        vTaskDelay(1000);
    }
}

void Task_SGP30(void *pvParameters)
{
    (void)pvParameters;

    if (!sgp.begin())
    {
        while (1)
        {
            Serial.println("SGP30 not found.");
            vTaskDelay(5000);
        }
    }

    while (1)
    {
        int humidity = humidity_value;
        int temperature = temperature_value;

        sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

        if (!sgp.IAQmeasure())
        {
            Serial.println("Measurement failed");
            continue;
        }
        Serial.print("TVOC ");
        Serial.print(tvoc_value = sgp.TVOC);
        Serial.print(" ppb\t");
        Serial.print("eCO2 ");
        Serial.print(co2_value = sgp.eCO2);
        Serial.println(" ppm");

        vTaskDelay(500);
    }
}

void Task_DHT11(void *pvParameters)
{
    (void)pvParameters;

    dht.begin();

    while (1) // A Task shall never return or exit.
    {
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if (isnan(h) || isnan(t))
        {
            Serial.println(F("Failed to read from DHT sensor!"));
        }
        else
        {
            Serial.print(F("Humidity: "));
            Serial.print(h);
            Serial.print(F("%  Temperature: "));
            Serial.print(t);
            Serial.print(F("°C "));
            humidity_value = (int)h;
            temperature_value = (int)t;
        }

        vTaskDelay(3000);
    }
}

void Task_BUZZ(void *pvParameters)
{
    (void)pvParameters;

    pinMode(BUZZPIN, OUTPUT);

    while (1)
    {
        digitalWrite(BUZZPIN, HIGH);
        vTaskDelay(500);
        digitalWrite(BUZZPIN, LOW);
        vTaskDelay(500);
        digitalWrite(BUZZPIN, HIGH);
        vTaskDelay(500);
        digitalWrite(BUZZPIN, LOW);

        vTaskDelay(2000);
    }
}

//Hardware init
void tft_init()
{
    panel.freq_write = 40000000;
    panel.freq_fill = 40000000;
    panel.freq_read = 16000000;

    panel.spi_cs = LCD_CS;
    panel.spi_dc = LCD_DC;
    panel.gpio_rst = LCD_RST;
    panel.gpio_bl = LCD_BL;

    tft.setPanel(&panel);
    tft.begin();
    tft.setRotation(SCRENN_ROTATION);
    tft.fillScreen(COLOR_BACKGROUND);
}

//UI function
void page_title(String title)
{
    tft.fillScreen(COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(20, 10);
    tft.setTextSize(4);
    tft.println(title);
}

uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
    return absoluteHumidityScaled;
}
