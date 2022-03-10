#ifndef _Makerfabs_Pin_H_
#define _Makerfabs_Pin_H_

#define DEBUG 1

#define I2C_SDA 26
#define I2C_SCL 27

//Touch

#define FT6236_TOUCH
//#define NS2009_TOUCH

#define SPI_MOSI 13
#define SPI_MISO 12
#define SPI_SCK 14

//TFT
#define LCD_MOSI SPI_MOSI
#define LCD_MISO SPI_MISO
#define LCD_SCK SPI_SCK
#define LCD_CS 15
#define LCD_RST 26
#define LCD_DC 33
#define LCD_BL -1

#define LCD_WIDTH 320
#define LCD_HEIGHT 480

#define SCRENN_ROTATION 3

//SD Card
#define SD_CS 4

//SPI control
// #define SPI_ON_TFT digitalWrite(LCD_CS, LOW)
// #define SPI_OFF_TFT digitalWrite(LCD_CS, HIGH)
// #define SPI_ON_SD digitalWrite(SD_CS, LOW)
// #define SPI_OFF_SD digitalWrite(SD_CS, HIGH)

#define SPI_ON_TFT 
#define SPI_OFF_TFT
#define SPI_ON_SD
#define SPI_OFF_SD 

//Lora
//HSPI
#define LORA_MOSI 23
#define LORA_MISO 19
#define LORA_SCK 18
#define LORA_CS 21

#define LORA_RST 22
#define LORA_DIO0 32
#define LORA_DIO1 35

#define FREQUENCY 434.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 10
#define PREAMBLE_LEN 8
#define GAIN 0


#endif