#include "cb.h"

CircularBuffer cb;

void cb_init(CircularBuffer *cb)
{
	cb->start = &(cb->buff[0]);
	cb->end = &(cb->buff[79]);
	cb->read = &(cb->buff[0]);
	cb->write = &(cb->buff[0]);
}

void cb_write(CircularBuffer *cb, char data)
{
	*(cb->write) = data;
	if(cb->write == cb->end)
		cb->write = cb->start;
	else
		cb->write++;
	
}

uint8_t cb_read(CircularBuffer *cb, char *data)
{
	if(cb->read != cb->write) {
		*data = *(cb->read);
		if(cb->read != cb->end)
			cb->read++;
		else
			cb->read = cb->start;
		return 1;
	}
	return 0;
}


void serial_write(char c) {
	while ( !( UCSR0A & ( 1 << UDRE0 ) ) );
	UDR0 = c;
}

void serial_writeln(char *line) {
	uint8_t i = 0;
	while(line[i] != '\0') {
		while ( !( UCSR0A & ( 1 << UDRE0 ) ) );
		UDR0 = line[i++];
	}
}

void uart_init( void ) {
	cb_init(&cb);
	UBRR0H = ( ( ( F_CPU / ( BAUD * 8UL ) ) - 1 ) >> 8 );
	UBRR0L = ( ( F_CPU / ( BAUD * 8UL ) ) - 1 );
	UCSR0A |= _BV( U2X0 );
	UCSR0B |= _BV( RXEN0 ) | _BV( TXEN0 ) | _BV( RXCIE0 );
	UCSR0C |= _BV( UCSZ00) | _BV( UCSZ01);
}

ISR(USART_RX_vect)
{
	char r = UDR0;
	cb_write(&cb, r);
}