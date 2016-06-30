#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "utils/ringbuf.h"

#include "uart_dev.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"

#define BUF_SIZE 128

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine) {
}
#endif

volatile unsigned int seconds;

void timer_handler(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    seconds += 1;
}

void write_hex(char c, uart_dev_t* dev);
void write_time(unsigned int seconds, uart_dev_t* dev);

int main(void) {
    char c = 0;
    unsigned int logging = 0;
    unsigned int num_read;
    unsigned int idx;
    int temp;
    char buf[BUF_SIZE];
    uart_dev_t *UART0_d;
    uart_dev_t *UART1_d;
    uart_dev_t *UART2_d;

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    RIT128x96x4Init(1000000);
    RIT128x96x4StringDraw("UART Bus Analyzer",    12,  0, 15);
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
    //Peripherals for TIMER1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Initialize uarts
    UART0_d = uart_dev_init(0, 115200);
    UART1_d = uart_dev_init(1, 115200);
    UART2_d = uart_dev_init(2, 115200);

    //Prompt User To enter time
    uart_dev_write(UART0_d, "Enter hours: \r\n", 15);
    idx = 0;
    while(1) {
        num_read = uart_dev_read(UART0_d, &c, 1);
        if(num_read) {
            if(c == '\n' || c == '\r') {
                break;
            } else if((c >= '0') && (c <= '9')) {
                if(idx < 2) {
                    buf[idx++] = c;
                    uart_dev_write(UART0_d, &c, 1);
                }
            } else {
                idx = 0;
                uart_dev_write(UART0_d, "\r  \r", 4);
            }
        }
    }
    temp = 1;
    seconds = 0;
    while(idx--) {
        seconds += temp*3600*(buf[idx]-'0');
        temp *= 10;
    }

    //Prompt User To enter time
    uart_dev_write(UART0_d, "\r\nEnter minutes: \r\n", 17);
    idx = 0;
    while(1) {
        num_read = uart_dev_read(UART0_d, &c, 1);
        if(num_read) {
            if(c == '\n' || c == '\r') {
                break;
            } else if((c >= '0') && (c <= '9')) {
                if(idx < 2) {
                    buf[idx++] = c;
                    uart_dev_write(UART0_d, &c, 1);
                }
            } else {
                idx = 0;
                uart_dev_write(UART0_d, "\r  \r", 4);
            }
        }
    }
    temp = 1;
    while(idx--) {
        seconds += temp*60*(buf[idx]-'0');
        temp *= 10;
    }
    uart_dev_write(UART0_d, "\r\n", 2);

    // Initialize timer
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, 8000000);
    TimerIntRegister(TIMER1_BASE, TIMER_BOTH, timer_handler);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);

    // Enable processor interrupts.
    IntMasterEnable();

    // Don't fall off the world!
    while(1) {
        // Read logging flag from UART 0
        num_read = uart_dev_read(UART0_d, &c, 1);
        if(num_read && c == ' ') {
            logging ^= 1;
            uart_dev_write(UART0_d, "Logging Toggled\r\n", 17);
        }

        // Transfer data from UART2 to UART1
        num_read = uart_dev_read(UART2_d, buf, BUF_SIZE);
        if(num_read) {
            uart_dev_write(UART1_d, buf, num_read);
            if(logging) {
                uart_dev_write(UART0_d, "2 -> 1: ", 8);
                write_time(seconds, UART0_d);
                temp = 0;
                while(temp < num_read) {
                    write_hex(buf[temp], UART0_d);
                    temp++;
                }
                uart_dev_write(UART0_d, "\r\n", 2);
            }
        }
        // Transfer data from UART1 to UART2
        num_read = uart_dev_read(UART1_d, buf, BUF_SIZE);
        if(num_read) {
            uart_dev_write(UART2_d, buf, num_read);
            if(logging) {
                uart_dev_write(UART0_d, buf, num_read);
            }
        }
    }
}

void write_hex(char c, uart_dev_t* dev) {
    char top = (c>>4) & 0xF;
    char bot = (c) & 0xF;
    top += '0' + (top>=10?7:0);
    bot += '0' + (bot>=10?7:0);
    uart_dev_write(dev, "0x", 2);
    uart_dev_write(dev, &top, 1);
    uart_dev_write(dev, &bot, 1);
    uart_dev_write(dev, " \'", 2);
    uart_dev_write(dev, &c, 1);
    uart_dev_write(dev, "\' ", 2);
    return;
}

void write_time(unsigned int seconds, uart_dev_t* dev) {
    unsigned int hours = (((seconds/3600) - 1) % 12) + 1;
    unsigned int minutes = ((seconds/60) % 60);
    seconds = seconds % 60;
    char c[2];

    c[0] = (hours/10) + '0';
    c[1] = (hours%10) + '0';
    uart_dev_write(dev, c, 2);
    uart_dev_write(dev, ":", 1);

    c[0] = (minutes/10) + '0';
    c[1] = (minutes%10) + '0';
    uart_dev_write(dev, c, 2);
    uart_dev_write(dev, ":", 1);

    c[0] = (seconds/10) + '0';
    c[1] = (seconds%10) + '0';
    uart_dev_write(dev, c, 2);
    uart_dev_write(dev, " ", 1);
    return;
}
