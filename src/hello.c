#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "utils/ringbuf.h"

#include "uart_dev.h"
#include "driverlib/uart.h"

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine) {
}
#endif

int main(void) {
    uart_dev_t *UART0_d;
    uart_dev_t *UART1_d;
    uart_dev_t *UART2_d;

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

    // peripherals for UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // peripherals for UART1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    // peripherals for UART2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    // Initialize uarts
    UART0_d = uart_dev_init(0, 115200);
    // UART1_d = uart_dev_init(1, 115200);
    // UART2_d = uart_dev_init(2, 115200);

    // Enable processor interrupts.
    IntMasterEnable();

    // uart_dev_send(UART0_d, str, strlen((char *)str));
    uart_dev_send(UART0_d, "Hello world, this is a fairly long string, much longer than the hardware buffer would allow. How are you today? I am doing go\n\r", 127);
    //uart_dev_init(&UART1, UART1_BASE);
    //uart_dev_init(&UART2, UART2_BASE);

    // Don't fall off the world!
    while(1) {
        // wait on interface
        // toggle LED
    }
}

