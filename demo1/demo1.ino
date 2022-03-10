#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include <SPI.h>
#include <LovyanGFX.hpp>

#include "makerfabs_pin.h"
#include "touch.h"
#include "Button.h"

#define PIN_A 5
#define PIN_B 18
#define PIN_C 19
#define PIN_D 21
#define LED_FRESH_TIME 20

#define COLOR_BACKGROUND 0xEF9E
#define COLOR_BUTTON TFT_WHITE
#define COLOR_TEXT 0x322B
#define COLOR_LINE TFT_BLACK
#define COLOR_SHADOW 0x4BAF

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
String test_str = "";
int my_count = 0;
int b_a_status = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LCD_CS, OUTPUT);

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_Count, "Task_Count", 2048, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_GPIO_1, "Task_GPIO_1", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_GPIO_2, "Task_GPIO_2", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_GPIO_3, "Task_GPIO_3", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(Task_GPIO_4, "Task_GPIO_4", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
}

void loop()
{
}

void Task_TFT(void *pvParameters) // This is a task.
{
    (void)pvParameters;

    tft_init();
    touch_init();

    while (1) // A Task shall never return or exit.
    {
        test_page();
    }
}

void Task_Count(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        if (b_a_status % 2)
        {
            my_count++;
            test_str = (String) "Count:" + my_count;
        }
        vTaskDelay(1000);
    }
}

void Task_GPIO_1(void *pvParameters)
{
    (void)pvParameters;

    ledcAttachPin(PIN_A, 0); // assign RGB led pins to channels
    ledcSetup(0, 1000, 8);   // 12 kHz PWM, 8-bit resolution

    while (1) // A Task shall never return or exit.
    {
        for (int i = 0; i < 256; i += 16)
        {
            ledcWrite(0, i);
            vTaskDelay(LED_FRESH_TIME);
        }
        for (int i = 255; i >= 0; i -= 16)
        {
            ledcWrite(0, i);
            vTaskDelay(LED_FRESH_TIME);
        }
    }
}
void Task_GPIO_2(void *pvParameters)
{
    (void)pvParameters;

    ledcAttachPin(PIN_B, 1); // assign RGB led pins to channels
    ledcSetup(1, 1000, 8);   // 12 kHz PWM, 8-bit resolution

    while (1) // A Task shall never return or exit.
    {
        for (int i = 0; i < 256; i += 16)
        {
            ledcWrite(1, i);
            vTaskDelay(2 * LED_FRESH_TIME);
        }
        for (int i = 255; i >= 0; i -= 16)
        {
            ledcWrite(1, i);
            vTaskDelay(2 * LED_FRESH_TIME);
        }
    }
}
void Task_GPIO_3(void *pvParameters)
{
    (void)pvParameters;

    pinMode(PIN_C, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_C, HIGH);
        vTaskDelay(500);
        digitalWrite(PIN_C, LOW);
        vTaskDelay(500);
    }
}

void Task_GPIO_4(void *pvParameters)
{
    (void)pvParameters;
    pinMode(PIN_D, OUTPUT);

    while (1) // A Task shall never return or exit.
    {
        digitalWrite(PIN_D, HIGH);
        vTaskDelay(500);
        digitalWrite(PIN_D, LOW);
        vTaskDelay(1000);
        digitalWrite(PIN_D, HIGH);
        vTaskDelay(1500);
        digitalWrite(PIN_D, LOW);
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

//Page
void test_page()
{
    Button b_a;
    Button b_b;

    b_a.set(300, 200, 150, 100, "START", ENABLE);
    b_a.setTextSize(2);

    int pos[2] = {0, 0};

    page_title("FreeRTOS Demo 1");

    while (1)
    {
        drawButton(b_a);

        vTaskDelay(200);

        int count = 0;
        while (1)
        {
            if (count < 5)
            {
                count++;
            }
            else
            {
                count = 0;
                tft.fillRect(40, 80, 200, 40, COLOR_BACKGROUND);
                tft.setCursor(40, 80);
                tft.setTextColor(COLOR_TEXT);
                tft.setTextSize(2);
                tft.println(test_str);
            }
            if (get_touch(pos))
            {
                int button_value = UNABLE;

                if ((button_value = b_a.checkTouch(pos[0], pos[1])) != UNABLE)
                {
                    if (DEBUG)
                    {
                        Serial.println("a Button");
                    }
                    if (b_a_status++ % 2 == 0)
                    {
                        b_a.setText("STOP");
                    }
                    else
                    {
                        b_a.setText("START");
                    }

                    break;
                }
            }
            vTaskDelay(50);
        }
    }
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

void drawButton(Button b)
{
    int b_x;
    int b_y;
    int b_w;
    int b_h;
    int shadow_len = 4;
    String text;
    int textSize;

    b.getFoDraw(&b_x, &b_y, &b_w, &b_h, &text, &textSize);

    tft.fillRect(b_x, b_y, b_w, b_h, COLOR_BUTTON);
    tft.drawRect(b_x, b_y, b_w, b_h, COLOR_LINE);
    tft.setCursor(b_x + 20, b_y + 20);
    tft.setTextColor(COLOR_TEXT);
    tft.setTextSize(textSize);
    tft.println(text);

    //Add button shadow
    if (b.getValue() != UNABLE)
    {
        tft.fillRect(b_x + shadow_len, b_y + b_h, b_w, shadow_len, COLOR_SHADOW);
        tft.fillRect(b_x + b_w, b_y + shadow_len, shadow_len, b_h, COLOR_SHADOW);
    }
}