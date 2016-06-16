#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

#include "uart_dev.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine) {
}
#endif

void init();

int main(void) {
    init();

    // Don't fall off the world!
    while(1) {
        // wait on interface
        // toggle LED
    }
}

void init() {
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display.
    RIT128x96x4Init(1000000);
}
