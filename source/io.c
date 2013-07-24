#include <xc.h>
#include "io.h"

inline void led_blinkStatus(void);

void io_processLoop(void) {

    led_blinkStatus();

}

inline void led_blinkStatus(void) {

    static unsigned int i = 0;

    i++;

    if (i >= 65000u) {

        i = 0;
        LED0_toggle();

    }

}

