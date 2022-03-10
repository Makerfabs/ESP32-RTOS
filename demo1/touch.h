#ifndef _Touch_H_
#define _Touch_H_

#include <arduino.h>
#include "makerfabs_pin.h"

#ifdef FT6236_TOUCH
#include "FT6236.h"
#endif


void touch_init();
int get_touch(int pos[2]);
void pos_rotation(int pos[2], int rotation);

#endif