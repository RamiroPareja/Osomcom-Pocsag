#include <xc.h>
#include "io.h"


void led_processLoop(void) {

    led_blinkStatus();

}

void led_blinkStatus(void) {

    static unsigned int i = 0;

    i++;

    if (i >= 65000u) {

        i = 0;
        LED0_toggle();

    }

}

