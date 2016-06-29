#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "utils/ringbuf.h"

#include "uart_dev.h"

// global device structures for each possible UART
uart_dev_t UART0_d;
uart_dev_t UART1_d;
uart_dev_t UART2_d;

void handle(uart_dev_t *dev) {
    unsigned long ulStatus;
    unsigned char c;

    // Get the interrrupt status.
    ulStatus = UARTIntStatus(dev->base_addr, true);

    // Clear the asserted interrupts.
    UARTIntClear(dev->base_addr, ulStatus);

    // Loop while there are characters in the receive FIFO.
    while(UARTCharsAvail(dev->base_addr) && !RingBufFull((tRingBufObject *) &(dev->rx_buf))) {
        // read from hardware buffer, put in rx_rb
        c = (unsigned char)UARTCharGetNonBlocking(dev->base_addr);
        RingBufWrite((tRingBufObject *) &dev->rx_buf, &c, 1);
    }

    while(UARTSpaceAvail(dev->base_addr) && !RingBufEmpty((tRingBufObject *) &dev->tx_buf)) {
        // read from tx_rb, and put in hardware buffer
        RingBufRead((tRingBufObject *) &dev->tx_buf, &c, 1);
        UARTCharPutNonBlocking(dev->base_addr, c);
    }
}

void UART0IntHandler(void) {
    handle(&UART0_d);
}

void UART1IntHandler(void) {
    handle(&UART1_d);
}

void UART2IntHandler(void) {
    handle(&UART2_d);
}


uart_dev_t *uart_dev_init(int id, int baud_rate) {
    uart_dev_t *dev;

    if (id == 0) {
        dev = &UART0_d;
        dev->base_addr = UART0_BASE;

        // Register a UART interrupt handler
        UARTIntRegister(dev->base_addr, UART0IntHandler);
        // Set GPIO A0 and A1 as UART pins.
        GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    } else if (id == 1) {
        dev = &UART1_d;
        dev->base_addr = UART1_BASE;

        // Register a UART interrupt handler
        UARTIntRegister(dev->base_addr, UART1IntHandler);
        // Set GPIO D2 and D3 as UART pins
        GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    } else if (id == 2) {
        dev = &UART2_d;
        dev->base_addr = UART2_BASE;

        // Register a UART interrupt handler
        UARTIntRegister(dev->base_addr, UART2IntHandler);
        // Set GPIO G0 and G1 as UART pins.
        GPIOPinTypeUART(GPIO_PORTG_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    } else {
        return 0;
    }

    // initialize both ring buffers
    RingBufInit((tRingBufObject *)&dev->tx_buf, (unsigned char *)dev->tx, UART_BUFFER_SIZE);
    RingBufInit((tRingBufObject *)&dev->rx_buf, (unsigned char *)dev->rx, UART_BUFFER_SIZE);

    UARTConfigSetExpClk(dev->base_addr, SysCtlClockGet(), baud_rate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // trigger TX interrupts when transmit is idle
    UARTTxIntModeSet(dev->base_addr, UART_TXINT_MODE_FIFO);
    UARTFIFOLevelSet(dev->base_addr, UART_FIFO_TX1_8, UART_FIFO_RX4_8);

    // Enable UART interrupts on TX, RX, and recieve buffer timeout
    UARTIntEnable(dev->base_addr, UART_INT_RX | UART_INT_RT | UART_INT_TX);

    // Return an address to the UART device structure
    return dev;
}


int uart_dev_write(uart_dev_t *dev, char *pucBuffer, unsigned long ulCount) {
    unsigned char c;
    unsigned long bytes_to_send;
    unsigned long bytes_written = 0;

    // Loop while there are more characters to send.
    while(!RingBufFull((tRingBufObject *) &(dev->tx_buf)) && ulCount--) {
        // Write the next character to the UART.
        // UARTCharPutNonBlocking(dev->base_addr, *pucBuffer++);
        bytes_written++;
        RingBufWrite((tRingBufObject *) &dev->tx_buf, (unsigned char *)pucBuffer++, 1);
    }

    if (!UARTBusy(dev->base_addr)) {
        // kick off the first write
        bytes_to_send = RingBufUsed((tRingBufObject *) &dev->tx_buf);
        if(bytes_to_send > 4){
            bytes_to_send = 4;
        }
        while(bytes_to_send--)
        {
            RingBufRead((tRingBufObject *) &dev->tx_buf, &c, 1);
            UARTCharPutNonBlocking(dev->base_addr, c);
        }
    }
    return bytes_written;
}

int uart_dev_read(uart_dev_t *dev, char *pucBuffer, unsigned long numChars) {
    unsigned long bytes_to_read;

    //Read how ever many bytes were requested or number in ring buffer, whatever is smaller
    bytes_to_read = RingBufUsed((tRingBufObject *) &dev->rx_buf);
    if(bytes_to_read > numChars) {
        bytes_to_read = numChars;
    }

    RingBufRead((tRingBufObject *) &dev->rx_buf, (unsigned char *)pucBuffer, bytes_to_read);
    return bytes_to_read;
}
