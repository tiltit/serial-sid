#include "helpers.h"

uint8_t hex2bin(char hi_nibble, char lo_nibble)
{
	uint8_t result;

	switch (hi_nibble) {
	    case '0': result = 0x00; break;
	    case '1': result = 0x10; break;
	    case '2': result = 0x20; break;
	    case '3': result = 0x30; break;
	    case '4': result = 0x40; break;
	    case '5': result = 0x50; break;
	    case '6': result = 0x60; break;
	    case '7': result = 0x70; break;
	    case '8': result = 0x80; break;
	    case '9': result = 0x90; break;
	    case 'a': result = 0xA0; break;
	    case 'A': result = 0xA0; break;
	    case 'b': result = 0xB0; break;
	    case 'B': result = 0xB0; break;
	    case 'c': result = 0xC0; break;
	    case 'C': result = 0xC0; break;
	    case 'd': result = 0xD0; break;
	    case 'D': result = 0xD0; break;
	    case 'e': result = 0xE0; break;
	    case 'E': result = 0xE0; break;
	    case 'f': result = 0xF0; break;
	    case 'F': result = 0xF0; break;
	    default : break;
	}

	switch (lo_nibble) {
	    case '0': result |= 0x00; break;
	    case '1': result |= 0x01; break;
	    case '2': result |= 0x02; break;
	    case '3': result |= 0x03; break;
	    case '4': result |= 0x04; break;
	    case '5': result |= 0x05; break;
	    case '6': result |= 0x06; break;
	    case '7': result |= 0x07; break;
	    case '8': result |= 0x08; break;
	    case '9': result |= 0x09; break;
	    case 'a': result |= 0x0A; break;
	    case 'A': result |= 0x0A; break;
	    case 'b': result |= 0x0B; break;
	    case 'B': result |= 0x0B; break;
	    case 'c': result |= 0x0C; break;
	    case 'C': result |= 0x0C; break;
	    case 'd': result |= 0x0D; break;
	    case 'D': result |= 0x0D; break;
	    case 'e': result |= 0x0E; break;
	    case 'E': result |= 0x0E; break;
	    case 'f': result |= 0x0F; break;
	    case 'F': result |= 0x0F; break;
	    default : break;
	}

	return result;
}