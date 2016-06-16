#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "utils/ringbuf.h"

#include "uart_dev.h"

uart_dev_t UART0_d;
uart_dev_t UART1_d;
uart_dev_t UART2_d;

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine) {
}
#endif

int main(void) {
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    RIT128x96x4Init(1000000);
    RIT128x96x4StringDraw("UART Echo",            36,  0, 15);
    RIT128x96x4StringDraw("Port:   Uart 0",       12, 16, 15);
    RIT128x96x4StringDraw("Baud:   115,200 bps",  12, 24, 15);
    RIT128x96x4StringDraw("Data:   8 Bit",        12, 32, 15);
    RIT128x96x4StringDraw("Parity: None",         12, 40, 15);
    RIT128x96x4StringDraw("Stop:   1 Bit",        12, 48, 15);

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable processor interrupts.
    IntMasterEnable();

    // Initialize uarts
    uart_dev_init(&UART0_d, UART0_BASE);

    uart_dev_send(&UART0_d, "Hello world", 10);
    //uart_dev_init(&UART1, UART1_BASE);
    //uart_dev_init(&UART2, UART2_BASE);

    // Don't fall off the world!
    while(1) {
        // wait on interface
        // toggle LED
    }
}

