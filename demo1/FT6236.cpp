#include "FT6236.h"

int readTouchReg(int reg)
{
    int data = 0;
    Wire.beginTransmission(TOUCH_I2C_ADD);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(TOUCH_I2C_ADD, 1);
    if (Wire.available())
    {
        data = Wire.read();
    }
    return data;
}

int getTouchPointX()
{
    int XL = 0;
    int XH = 0;

    XH = readTouchReg(TOUCH_REG_XH);
    //Serial.println(XH >> 6,HEX);
    if (XH >> 6 == 1)
        return -1;
    XL = readTouchReg(TOUCH_REG_XL);

    return ((XH & 0x0F) << 8) | XL;
}

int getTouchPointY()
{
    int YL = 0;
    int YH = 0;

    YH = readTouchReg(TOUCH_REG_YH);
    YL = readTouchReg(TOUCH_REG_YL);

    return ((YH & 0x0F) << 8) | YL;
}

void ft6236_init()
{
    Wire.begin(I2C_SDA, I2C_SCL);
    byte error, address;

    Wire.beginTransmission(TOUCH_I2C_ADD);
    error = Wire.endTransmission();

    if (error == 0)
    {
        Serial.print("I2C device found at address 0x");
        Serial.print(TOUCH_I2C_ADD, HEX);
        Serial.println("  !");
    }
    else if (error == 4)
    {
        Serial.print("Unknown error at address 0x");
        Serial.println(TOUCH_I2C_ADD, HEX);
    }
}

int ft6236_pos(int pos[2])
{
    int XL = 0;
    int XH = 0;
    int YL = 0;
    int YH = 0;

    XH = readTouchReg(TOUCH_REG_XH);
    if (XH >> 6 == 1)
    {
        pos[0] = -1;
        pos[1] = -1;

        return 0;
    }
    XL = readTouchReg(TOUCH_REG_XL);
    YH = readTouchReg(TOUCH_REG_YH);
    YL = readTouchReg(TOUCH_REG_YL);

    pos[0] = ((XH & 0x0F) << 8) | XL;
    pos[1] = ((YH & 0x0F) << 8) | YL;

    if (pos[0] == 4095)
    {
        pos[0] = -1;
        pos[1] = -1;

        return 0;
    }
    return 1;
}