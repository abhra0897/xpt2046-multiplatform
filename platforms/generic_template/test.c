#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "xpt2046.h"

int main(void)
{
    printf("This code does nothing!. It just includes the touch library and calls xpt2046_get_touch()\n");

    uint16_t x = 0, y = 0;
    xpt2046_get_touch(&x, &y);
    printf("X: %d, Y: %d\n", x, y);
    return 0;
}
