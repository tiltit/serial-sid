#ifndef CB_H
#define CB_H

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD 57600

typedef struct {
	char *start;
	char *end;
	char *read;
	char *write;
	char buff[250];
	
} CircularBuffer;

extern CircularBuffer cb;

void cb_init(CircularBuffer *cb);
uint8_t cb_read(CircularBuffer *cb, char *data);
void cb_write(CircularBuffer *cb, char data);
void serial_write(char c);
void serial_writeln(char *line);
void uart_init( void );
ISR(USART_RX_vect);

#endif
