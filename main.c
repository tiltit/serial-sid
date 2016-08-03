#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "cb.h"
#include "helpers.h"

// Second version of the board has a slitly diferent pin config
#define BOARD_V2


/* Sid registers */

// Voice Base registers
#define SID_VOICE_1              0x00
#define SID_VOICE_2              0x07
#define SID_VOICE_3              0x0E

// To access the sid Voice regiter add the voice base register to the folowing addresses
#define SID_VOICE_FREQ_LO        0x00
#define SID_VOICE_FREQ_HI        0x01
#define SID_VOICE_PW_LO          0x02
#define SID_VOICE_PW_HI          0x03
#define SID_VOICE_CONTROL        0x04
#define SID_VOICE_ATK_DECK       0x05
#define SID_VOICE_SUS_REL        0x06

// Filter registers
#define SID_FILTER_FC_LO         0x15
#define SID_FILTER_FC_HI         0x16
#define SID_FILTER_FC_RES_FILT   0x17
#define SID_FILTER_FC_MODE_VOL   0x18

// Misc registers
#define SID_MISC_POTX            0x19
#define SID_MISC_POTY            0x1A
#define SID_MISC_OSC3            0x1B
#define SID_MISC_ENV3            0x1C


typedef enum Voice_control {
	VOICE_GATE         = 0x01,
	VOICE_SYNC         = 0x02,
	VOICE_RINGMOD      = 0x04,
	VOICE_TEST         = 0x08,
	VOICE_TRIANGLE     = 0x10,
	VOICE_SAWTOOTH     = 0x20,
	VOICE_SQUARE       = 0x40,
	VOICE_NOISE        = 0x80
} Voice_control;

typedef enum Res_filt {
	FILT_1             = 0x01,
	FILT_2             = 0x02,
	FILT_3             = 0x04,
	FILT_EX            = 0x08,
	RES_0              = 0x10,
	RES_1              = 0x20,
	RES_2              = 0x40,
	RES_3              = 0x80
} Res_filt;

typedef enum Mode_vol {
	VOL_0              = 0x01,
	VOL_1              = 0x02,
	VOL_2              = 0x04,
	VOL_3              = 0x08,
	MODE_LP            = 0x10,
	MODE_BP            = 0x20,
	MODE_HP            = 0x40,
	MODE_OFF           = 0x80
} Mode_vol;

Voice_control voice_1, voice_2, voice_3;

Res_filt res_filt;

Mode_vol mode_vol;



void setup_io()
{
	// PB0 -> PB7 : Sid data and address bus.
	DDRB = 0xFF;

	// PC0 : 74273 Clock
	DDRC |= _BV(0);

	// PC1 : 74273 Clear
	DDRC |= _BV(1);

	// PD4 : Sid reset
	DDRD |= _BV(4);

	// PD5 Sid Clock
	DDRD |= _BV(5); // OC0B

	// PD7 Sid cable select
	DDRD |= _BV(7);
}

void setup_sid_clock(void)
{	
	TCCR0A = _BV(COM0B0) | _BV(WGM01); //  Clear Timer on Compare Match with output
	
	TCCR0B = _BV(CS00); // No prescaling
	
	// Must use OCR0A for ctc mode
	OCR0A = 3;
}

void sid_set_address(uint8_t val)
{

#ifndef BOARD_V2
	// Pull 74273 reset pin low
	PORTC &= ~(_BV(1));
	//_delay_us(1);
	PORTB = (val & 0x1F); // Set address
	PORTB &= ~(0b00100000); // Set read/write flag low
	
	// Toggle 74273 Clock
	PORTC &= ~(_BV(0));
	
	//_delay_us(4);
	PORTC |= (_BV(1));
	//_delay_us(4);
	PORTC |= _BV(0);
	//_delay_us(4);
#else

	// Pull 74273 reset pin low
	PORTC &= ~(_BV(0));
	//_delay_us(1);
	PORTB = (val & 0x1F); // Set address
	PORTB &= ~(0b00100000); // Set read/write flag low
	
	// Toggle 74273 Clock
	PORTC &= ~(_BV(1));
	
	//_delay_us(4);
	PORTC |= (_BV(0));
	//_delay_us(4);
	PORTC |= _BV(1);
	//_delay_us(4);
#endif

}

inline void sid_set_data(uint8_t val)
{
	PORTB = val;
}

void sid_write(uint8_t addr, uint8_t data)
{
	cli();
	
	sid_set_address(addr);
	sid_set_data(data);
	
	// Lower and Raise CS pin
	PORTD &= ~(_BV(7));
	_delay_us(1);
	PORTD |= _BV(7);
	
	sei();
}

/*  Frequency cutoff 11 bit umber, max val = 2043 */
void sid_filter_set_fc(uint16_t val)
{
	sid_write(SID_FILTER_FC_LO, (uint8_t)(val & 0x0007) );
	sid_write(SID_FILTER_FC_HI, (uint8_t)(val >> 3) );
}

// Volume 0 -> 15
void sid_filter_set_vol(uint8_t val)
{
	mode_vol &= ~(0x0F);
	mode_vol |= (val & 0x0F);
	sid_write(SID_FILTER_FC_MODE_VOL, mode_vol);
}

void sid_filter_set_res(uint8_t res)
{
	res_filt &= ~(0xF0);
	res_filt |= ( (res << 4) & 0xF0 );
	sid_write(SID_FILTER_FC_RES_FILT, res_filt);
}

void sid_voice_set_freq(uint8_t voice, uint16_t freq) 
{
	sid_write(voice + SID_VOICE_FREQ_LO, (uint8_t)(freq)); 
	sid_write(voice + SID_VOICE_FREQ_HI, (uint8_t)(freq >> 8));	
}

void sid_filter_enable(uint8_t voice)
{
	switch(voice) {
		case SID_VOICE_1: res_filt |= FILT_1; break;
		case SID_VOICE_2: res_filt |= FILT_2; break;
		case SID_VOICE_3: res_filt |= FILT_3; break;
	}
	sid_write(SID_FILTER_FC_RES_FILT, res_filt);
}

void sid_voice_set_wave(uint8_t voice, uint8_t wave_type)
{
	switch(voice) {
		case SID_VOICE_1:
			voice_1 |= (wave_type);
			sid_write(voice + SID_VOICE_CONTROL, voice_1);
			break;
		case SID_VOICE_2:
			voice_2 |= (wave_type);
			sid_write(voice + SID_VOICE_CONTROL, voice_2);
			break;
		case SID_VOICE_3:
			voice_3 |= (wave_type);
			sid_write(voice + SID_VOICE_CONTROL, voice_3);
			break;
	}
}

// Voice pulse width for square wave
void sid_voice_set_pw(uint8_t voice, uint16_t val)
{
	sid_write(voice + SID_VOICE_PW_LO, (uint8_t)(val & 0x00FF) ); 
	sid_write(voice + SID_VOICE_PW_HI, (uint8_t)( (val >> 8 ) & 0x00FF) );
}

void sid_voice_set_attack_decay(uint8_t voice, uint8_t attack, uint8_t decay)
{
	sid_write(voice + SID_VOICE_ATK_DECK, ( (attack << 4) & 0xF0 ) + (decay & 0x0F)  ); 
}

void sid_voice_set_sustain_release(uint8_t voice, uint8_t sustain, uint8_t release)
{
	sid_write(voice + SID_VOICE_SUS_REL, ( (sustain << 4) & 0xF0 ) + (release & 0x0F )  ); 
}

void sid_start_attack(uint8_t voice)
{
	
	switch(voice) {
		case SID_VOICE_1:
			voice_1 |= (VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_1);
			break;
		case SID_VOICE_2:
			voice_2 |= (VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_2);
			break;
		case SID_VOICE_3:
			voice_3 |= (VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_3);
			break;
	}

}

void sid_start_release(uint8_t voice)
{
	switch(voice) {
		case SID_VOICE_1:
			voice_1 &= ~(VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_1);
			break;
		case SID_VOICE_2:
			voice_2 &= ~(VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_2);
			break;
		case SID_VOICE_3:
			voice_3 &= ~(VOICE_GATE);
			sid_write(voice + SID_VOICE_CONTROL, voice_3);
			break;
	}
}

void sid_beep()
{
	sid_voice_set_attack_decay(SID_VOICE_1, 9, 2);
	sid_voice_set_sustain_release(SID_VOICE_1, 2, 9);
	sid_start_attack(SID_VOICE_1);
	_delay_ms(400);
	
	sid_start_release(SID_VOICE_1);
	_delay_ms(100);
}

void serial_write_command(uint8_t *arg)
{
	char test[20];
	size_t arg_length = strlen(arg);
	
	
	if( (arg_length < 4) || (arg_length%2 != 0) ) {
		return;
	}

	uint8_t address = hex2bin(arg[0],arg[1]);

	uint8_t i;

	for(i=2;i<arg_length;i+=2) {
		sid_write(address, hex2bin(arg[i], arg[i+1]));
		address++;
	}
}

void process_command_line( uint8_t *line)
{
	uint8_t command;
	uint8_t *arg;
	
	command = line[0];
	arg = &line[1];

	switch (command) {
		case 'B': sid_beep(); break;
		case 'W':
			serial_write_command(arg);
			break;
		case 'R':
			// Reset sid
			PORTD &= ~(_BV(4));
			_delay_us(100);
			PORTD |= _BV(4);
			_delay_us(100);
			break;
		default: break;
	}
}

void read_serial( void )
{
	uint8_t incoming_data;
	static uint8_t command_line[100];
	static uint8_t i = 0;

	while(cb_read(&cb, &incoming_data)) {
		switch(incoming_data) {
			case '\r': 
				//command_line[i++] = '\0';
				break;
			case '\n':
				command_line[i++] = '\0'; 
				process_command_line(command_line);
				i = 0;
				break;
			default:
				command_line[i++] = incoming_data;
				i %= 100;
				break; 
		}
	}
}

int main (void)
{
	
	setup_io();
	uart_init();
	setup_sid_clock();
	sei();
	volatile uint8_t i;

	PORTD |= _BV(7); // Set Cable select high
	
	// Reset sid
	PORTD &= ~(_BV(4));
	_delay_us(100);
	PORTD |= _BV(4);
	_delay_us(100);

	mode_vol |= MODE_LP;
	sid_filter_set_vol(15);
	sid_voice_set_wave(SID_VOICE_1, VOICE_TRIANGLE);
	sid_voice_set_freq(SID_VOICE_1, 0x122A);
	sid_voice_set_pw(SID_VOICE_1, 2048); // 50%
	int filter_fc = 1023;

	while(1) {
		read_serial();
	}
	return 0;
}
