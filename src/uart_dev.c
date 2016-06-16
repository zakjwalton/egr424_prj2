#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "utils/ringbuf.h"

#include "uart_dev.h"

void UART0IntHandler(void) {
    unsigned long ulStatus;

    // Get the interrrupt status.
    ulStatus = UARTIntStatus(UART0_BASE, true);

    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ulStatus);

    // Loop while there are characters in the receive FIFO.
    while(UARTCharsAvail(UART0_BASE)) {
        // Read the next character from the UART and write it back to the UART.
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}


void uart_dev_init(uart_dev_t *dev, unsigned long base_addr) {
    dev->base_addr = base_addr;
    RingBufInit((tRingBufObject *)&dev->tx_buf, (unsigned char *)dev->tx, UART_BUFFER_SIZE);
    RingBufInit((tRingBufObject *)&dev->rx_buf, (unsigned char *)dev->rx, UART_BUFFER_SIZE);

    UARTConfigSetExpClk(base_addr, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    if (base_addr == UART0_BASE) {
        // Set GPIO A0 and A1 as UART pins.
        GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
        // Register a UART interrupt handler
        UARTIntRegister(base_addr, UART0IntHandler);
    }
    // Enable the UART interrupt.
    UARTIntEnable(base_addr, UART_INT_RX | UART_INT_RT);
}


void uart_dev_send(uart_dev_t *dev, const char *pucBuffer, unsigned long ulCount) {
    // Loop while there are more characters to send.
    while(ulCount--) {
        // Write the next character to the UART.
        UARTCharPutNonBlocking(dev->base_addr, *pucBuffer++);
    }
}

