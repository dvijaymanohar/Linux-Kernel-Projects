/* Modified Temp. Header File */

#ifndef UART_IO_H
#define UART_IO_H

void operation(unsigned char, unsigned);
void set_DLAB(unsigned char);
void read_reg(unsigned);
void reset_bit(unsigned char, unsigned);
void set_bit(unsigned char, unsigned);
void write_reg(unsigned, unsigned);

#define write_reg(value, port) outb(value, port);
#define BASE_ADDR 0x03F8
#define LCR_SBC_MASK 0xBF
#define LCR_PARITY_MASK 0xF7
#define MCR_LOOP_MASK 0xEF
#define MCR_RTS_MASK 0xFD
#define MCR_DTR_MASK 0xFE
#define MSR_DSR_MASK 0xDF
#define MSR_CTS_MASK 0xEF

#endif
