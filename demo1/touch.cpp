#include "touch.h"

void touch_init()
{
#ifdef FT6236_TOUCH
    ft6236_init();
#endif
}

int get_touch(int pos[2])
{
    int temp = 0;
#ifdef FT6236_TOUCH
    temp = ft6236_pos(pos);
#endif
    pos_rotation(pos, SCRENN_ROTATION);
    return temp;
}

//transform touch screen pos
void pos_rotation(int pos[2], int rotation)
{
    if (pos[0] == -1)
        return;
    if (rotation == 0)
    {
        return;
    }
    if (rotation == 3)
    {
        int tempx = 480 - pos[1];
        int tempy = pos[0];
        pos[0] = tempx;
        pos[1] = tempy;
    }
}