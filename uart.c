/**
 *
 * File Name: uart.c
 * Title    : UART library source
 * Project  : libuart
 * Author   : Copyright (C) 2018 krjdev@gmail.com
 * Created  : 2018-05-21
 * Modified : 2018-05-25
 * Revised  : 
 * Version  : 0.1.0.0
 * License  : BSD-3-Clause (see file LICENSE.txt)
 *
 * NOTE: This code is currently below version 1.0, and therefore is considered
 * to be lacking in some functionality or documentation, or may not be fully
 * tested. Nonetheless, you can expect most functions to work.
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "uart.h"
#include "error.h"

static int enum_contains(int enum_values[], int len, int value)
{
    int i;
    
    for (i = 0; i < len; i++)
        if (enum_values[i] == value)
            return 1;
    
    return 0;
}

static int is_baud_valid(int value)
{
    int E[] = {
        UART_BAUD_0,
        UART_BAUD_50,
        UART_BAUD_75,
        UART_BAUD_110,
        UART_BAUD_134,
        UART_BAUD_150,
        UART_BAUD_200,
        UART_BAUD_300,
        UART_BAUD_600,
        UART_BAUD_1200,
        UART_BAUD_1800,
        UART_BAUD_2400,
        UART_BAUD_4800,
        UART_BAUD_9600,
        UART_BAUD_19200,
        UART_BAUD_38400,
        UART_BAUD_57600,
        UART_BAUD_115200
    };
    
    if (enum_contains(E, sizeof(E)/sizeof(E[0]), value))
        return 1;
    
    return 0;
}

static int is_databits_valid(int value)
{
    if ((value > 4) && (value < 9))
        return 1;
    
    return 0;
}

static int is_parity_valid(int value)
{
    int ret = 0;
    
    switch (value) {
    case UART_PARITY_NO:
        ret = 1;
        break;
    case UART_PARITY_ODD:
        ret = 1;
        break;
    case UART_PARITY_EVEN:
        ret = 1;
        break;
    default:
        break;
    }
    
    return ret;
}

static int is_stopbits_valid(int value)
{
    if ((value > 0) && (value < 3))
        return 1;
    
    return 0;
}

static int is_flow_valid(int value)
{
    int ret = 0;
    
    switch (value) {
    case UART_FLOW_NO:
        ret = 1;
        break;
    case UART_FLOW_SOFTWARE:
        ret = 1;
        break;
    case UART_FLOW_HARDWARE:
        ret = 1;
        break;
    default:
        break;
    }
    
    return ret;
}

static int parse_option(uart_t *uart, const char *opt)
{
    int i = 0;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
    if (!opt) {
        printerr_string_null();
        return -1;
    }
    
    while (opt[i] != '\0') {
        /* parse data bits */
        switch (opt[i]) {
        case '5':
            uart->data_bits = 5;
            break;
        case '6':
            uart->data_bits = 6;
            break;
        case '7':
            uart->data_bits = 7;
            break;
        case '8':
            uart->data_bits = 8;
            break;
        default:
            printerr_uart_opt_invalid("data bits");
            return -1;
        }
        
        i++;
        
        /* parse parity */
        switch (opt[i]) {
        case 'N':
            uart->parity = UART_PARITY_NO;
            break;
        case 'O':
            uart->parity = UART_PARITY_ODD;
            break;
        case 'E':
            uart->parity = UART_PARITY_EVEN;
            break;
        default:
            printerr_uart_opt_invalid("parity");
            return -1;
        }
        
        i++;
        
        /* parse stop bits */
        switch (opt[i]) {
        case '1':
            uart->stop_bits = 1;
            break;
        case '2':
            uart->stop_bits = 2;
            break;
        default:
            printerr_uart_opt_invalid("stop bits");
            return -1;
        }
        
        i++;
        
        /* parse flow control */
        switch (opt[i]) {
        case 'N':
            uart->flow_ctrl = UART_FLOW_NO;
            break;
        case 'S':
            uart->flow_ctrl = UART_FLOW_SOFTWARE;
            break;
        case 'H':
            uart->flow_ctrl = UART_FLOW_HARDWARE;
            break;
        default:
            printerr_uart_opt_invalid("flow control");
            return -1;
        }
        
        i++;
        
        if (opt[i] != '\0') {
            printerr_uart_opt_invalid(NULL);
            return -1;
        }
    }
    
    return 0;
}

static int init_baud(uart_t *uart)
{
    int ret;
    struct termios options;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    switch (uart->baud) {
    case UART_BAUD_0:
        ret = cfsetispeed(&options, B0);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B0);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_50:
        ret = cfsetispeed(&options, B50);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B50);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_75:
        ret = cfsetispeed(&options, B75);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B75);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_110:
        ret = cfsetispeed(&options, B110);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B110);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_134:
        ret = cfsetispeed(&options, B134);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B134);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_150:
        ret = cfsetispeed(&options, B150);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B150);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_200:
        ret = cfsetispeed(&options, B200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_300:
        ret = cfsetispeed(&options, B300);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B300);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_600:
        ret = cfsetispeed(&options, B600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_1200:
        ret = cfsetispeed(&options, B1200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_2400:
        ret = cfsetispeed(&options, B2400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B2400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_4800:
        ret = cfsetispeed(&options, B4800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B4800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_9600:
        ret = cfsetispeed(&options, B9600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B9600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_19200:
        ret = cfsetispeed(&options, B19200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B19200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_38400:
        ret = cfsetispeed(&options, B38400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B38400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_57600:
        ret = cfsetispeed(&options, B57600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B57600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    case UART_BAUD_115200:
        ret = cfsetispeed(&options, B115200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B115200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        break;
    default:
        break;
    }
    
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

static int init_databits(uart_t *uart)
{
    int ret;
    struct termios options;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    switch (uart->data_bits) {
    case 5:
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        break;
    default:
        break;
    }
    
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

static int init_parity(uart_t *uart)
{
    int ret;
    struct termios options;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    switch (uart->parity) {
    case UART_PARITY_NO:
        options.c_cflag &= ~PARENB;
        break;
    case UART_PARITY_ODD:
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        break;
    case UART_PARITY_EVEN:
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    default:
        break;
    }
    
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

static int init_stopbits(uart_t *uart)
{
    int ret;
    struct termios options;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    switch (uart->stop_bits) {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        break;
    }
    
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

static int init_flow(uart_t *uart)
{
    int ret;
    struct termios options;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    switch (uart->flow_ctrl) {
    case UART_FLOW_NO:
        options.c_cflag &= ~CRTSCTS;
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;
    case UART_FLOW_SOFTWARE:
        options.c_cflag &= ~CRTSCTS;
        options.c_iflag |= (IXON | IXOFF | IXANY);
        break;
    case UART_FLOW_HARDWARE:
        options.c_cflag |= CRTSCTS;
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        break;
    default:
        break;
    }
    
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

static int init(uart_t *uart)
{
    int ret;
    struct termios options;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
    /* set non-blocking mode*/
    ret = fcntl(uart->fd, F_SETFL, FNDELAY);
    
    if (ret == -1) {
        printerr_fcntl(strerror(errno));
        return -1;
    }
    
    /* set baud rate */
    ret = init_baud(uart);
    
    if (ret == -1)
        return -1;
    
    /* set data bits */
    ret = init_databits(uart);
    
    if (ret == -1)
        return -1;
    
    /* set parity */
    ret = init_parity(uart);
    
    if (ret == -1)
        return -1;
    
    /* set stop bits */
    ret = init_stopbits(uart);
    
    if (ret == -1)
        return -1;
    
    /* set flow control */
    ret = init_flow(uart);
    
    if (ret == -1)
        return -1;
    
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    /* set raw input mode */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    /* enable receiver and set local mode */
    options.c_cflag |= (CLOCAL | CREAD);
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    return 0;
}

uart_t *uart_open(const char *dev, int baud, const char *opt)
{
    int fd, ret;
    uart_t *p;
    
    p = malloc(sizeof(uart_t));
    
    if (!p) {
        printerr_alloc(strerror(errno));
        return NULL;
    }
    
    if (strlen(dev) >= BUFSZ) {
        printerr_uart_dev_too_long();
        return NULL;
    }
    
    strcpy(p->dev, dev);
    fd = open(p->dev, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (!fd) {
        printerr_open(strerror(errno));
        return NULL;
    }
    
    p->fd = fd;
    ret = parse_option(p, opt);
    
    if (ret == -1)
        return NULL;
    
    if (!is_baud_valid(baud)) {
        printerr_uart_baud_invalid();
        return NULL;
    }
    
    p->baud = baud;
    ret = init(p);
    
    if (ret == -1)
        return NULL;
    
    return p;
}

void uart_close(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    close(uart->fd);
    free(uart);
    uart = NULL;
}

int uart_send(uart_t *uart, char *send_buf, int len)
{
    int ret;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    ret = write(uart->fd, send_buf, len);
    
    if (ret == -1) {
        printerr_write(strerror(errno));
        return -1;
    }
    
    if (ret != len) {
        printerr_write("couldn't send all bytes");
        return ret;
    }
    
    return ret;
}

int uart_recv(uart_t *uart, char *recv_buf, int len)
{
    int ret = 0;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    ret = read(uart->fd, recv_buf, len);
    
    if (ret == -1) {
        printerr_read(strerror(errno));
        return -1;
    }
    
    return ret;
}

void uart_pin_set(uart_t *uart, int pin, int state)
{
    int status, ret;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    ret = ioctl(uart->fd, TIOCMGET, &status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return;
    }
    
    switch (pin) {
    case UART_PIN_RTS:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_RTS;
        else
            status &= ~TIOCM_RTS;
        break;
    case UART_PIN_CTS:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_CTS;
        else
            status &= ~TIOCM_CTS;
        break;
    case UART_PIN_DSR:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_DSR;
        else
            status &= ~TIOCM_DSR;
        break;
    case UART_PIN_DCD:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_CAR;
        else
            status &= ~TIOCM_CAR;
        break;
    case UART_PIN_DTR:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_DTR;
        else
            status &= ~TIOCM_DTR;
        break;
    case UART_PIN_RI:
        if (state == UART_PIN_HIGH)
            status |= TIOCM_RI;
        else
            status &= ~TIOCM_RI;
        break;
    default:
        printerr_uart_pin_invalid();
    }
    
    ioctl(uart->fd, TIOCMSET, status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return;
    }
}

int uart_pin_get(uart_t *uart, int pin)
{
    int ret = 0;
    int status;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    ioctl(uart->fd, TIOCMGET, &status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return -1;
    }
    
    switch (pin) {
    case UART_PIN_RTS:
        if (status & TIOCM_RTS)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_CTS:
        if (status & TIOCM_CTS)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_DSR:
        if (status & TIOCM_DSR)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_DCD:
        if (status & TIOCM_CAR)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_DTR:
        if (status & TIOCM_DTR)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_RI:
        if (status & TIOCM_RI)
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    default:
        printerr_uart_pin_invalid();
        ret = -1;
    }
    
    return ret;
}

int uart_bytes_get(uart_t *uart)
{
    int ret = 0;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    ioctl(uart->fd, FIONREAD, &ret);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return -1;
    }
    
    return ret;
}

void uart_baud_set(uart_t *uart, int baud)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_baud_valid(baud)) {
        printerr_uart_baud_invalid();
        return;
    }
    
    uart->baud = baud;    
    init_baud(uart);
}

int uart_baud_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->baud; 
}

int uart_fd_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
    return uart->fd;
}

char *uart_dev_get(uart_t *uart)
{
    char *p;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return NULL;
    }
    
    p = malloc(strlen(uart->dev) + 1);
    
    if (!p) {
        printerr_alloc(strerror(errno));
        return NULL;
    }
    
    strcpy(p, uart->dev);
    return p; 
}

void uart_databits_set(uart_t *uart, int data_bits)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_databits_valid(data_bits))
    {
        printerr_uart_data_invalid();
        return;
    }
    
    uart->data_bits = data_bits;
    init_databits(uart);
}

int uart_databits_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->data_bits;
}

void uart_parity_set(uart_t *uart, int parity)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_parity_valid(parity))
    {
        printerr_uart_parity_invalid();
        return;
    }
    
    uart->parity = parity;
    init_parity(uart);
}

int uart_parity_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->parity;
}

void uart_stopbits_set(uart_t *uart, int stop_bits)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_stopbits_valid(stop_bits))
    {
        printerr_uart_stop_invalid();
        return;
    }
    
    uart->stop_bits = stop_bits;
    init_stopbits(uart);
}

int uart_stopbits_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->stop_bits;
}

void uart_flow_set(uart_t *uart, int flow_ctrl)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_flow_valid(flow_ctrl))
    {
        printerr_uart_flow_invalid();
        return;
    }
    
    uart->flow_ctrl = flow_ctrl;
    init_flow(uart);
}

int uart_flow_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->flow_ctrl;
}
