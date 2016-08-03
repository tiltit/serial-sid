#$from enum import Enum
from enum import IntEnum
from typing import Dict
import serial, time
#import codecs

#import binascii

class SidRegisters(IntEnum):
	# Voice Base registers
	SID_VOICE_1             = 0x00
	SID_VOICE_2             = 0x07
	SID_VOICE_3             = 0x0E

	# To access the sid Voice regiter add the voice base register to the folowing addresses
	SID_VOICE_FREQ_LO       = 0x00
	SID_VOICE_FREQ_HI       = 0x01
	SID_VOICE_PW_LO         = 0x02
	SID_VOICE_PW_HI         = 0x03
	SID_VOICE_CONTROL       = 0x04
	SID_VOICE_ATK_DECK      = 0x05
	SID_VOICE_SUS_REL       = 0x06

	# Filter registers
	SID_FILTER_FC_LO        = 0x15
	SID_FILTER_FC_HI        = 0x16
	SID_FILTER_FC_RES_FILT  = 0x17
	SID_FILTER_FC_MODE_VOL  = 0x18

	# Misc registers
	SID_MISC_POTX           = 0x19
	SID_MISC_POTY           = 0x1A
	SID_MISC_OSC3           = 0x1B
	SID_MISC_ENV3           = 0x1C


class Voice():
	def __init__(self, voice_address):
		self.base_address = voice_address
		self.freq = 0
		self.pulse_width = 2048
		

class Sid():
	""" Sid class for controling the 6581 ### """
	def __init__(self, serial_port):
		
		self.voice_1 = Voice(SidRegisters.SID_VOICE_1)
		self.voice_2 = Voice(SidRegisters.SID_VOICE_2)
		self.voice_3 = Voice(SidRegisters.SID_VOICE_3)

		self.voice_map = [SidRegisters.SID_VOICE_1, SidRegisters.SID_VOICE_2, SidRegisters.SID_VOICE_3]
		
		### The sid regigisters state
		self.sid_state = bytearray(29)

		self.ser = serial.Serial(serial_port, 57600)
		time.sleep(2)

	def send_command(self, command):
		self.ser.write(bytearray(command + "\n", 'ascii'))

	def sid_reset(self):
		self.send_command("R")
		for i in range(0, 29):
			self.sid_state[i] = 0
			time.sleep(0.1)

	def voice_set_freq(self, voice: int,freq: int):
		self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_FREQ_LO] = (freq & 0xFF)
		self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_FREQ_HI] = ((freq >> 8) & 0xFF)
		address = self.voice_map[voice] + SidRegisters.SID_VOICE_FREQ_LO
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address]) + '{:02x}'.format(self.sid_state[address+1])
		self.send_command(command)

	def voice_set_pulse_width( self, voice: int, pulse_width: int):
		address = self.voice_map[voice] + SID_FILTER_FC_LO
		self.sid_state[address] = pulse_width & 0xFF
		self.sid_state[address+1] = (pulse_width >> 8) & 0x0F
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address]) + '{:02x}'.format(self.sid_state[address+1])
		self.send_command(command)


	def voice_sync(self, voice: int, enabled: bool):
		if enabled == True:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b00000010			
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b00000010)	

	def voice_ring(self, voice: int, enabled: bool):
		if enabled == True:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b00000010
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b00000010)

	def voice_waveform_noise(self, voice: int, enabled: bool):
		if enabled:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b10000000
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b10000000)

	def voice_waveform_square(self, voice: int, enabled: bool):
		if enabled:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b01000000
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b01000000)

	def voice_waveform_sawtooth(self, voice: int, enabled: bool):
		if enabled:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b00100000
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b00100000)

	def voice_waveform_triangle(self, voice: int, enabled: bool):
		if enabled:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] |= 0b00010000
		else:
			self.sid_state[self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL] &= ~(0b00010000)

	def voice_set_attack_decay(self, voice: int, attack: int, decay: int):
		address = self.voice_map[voice] + SidRegisters.SID_VOICE_ATK_DECK
		self.sid_state[address] = (attack % 16) << 4;
		self.sid_state[address] += (decay % 16)
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address])
		self.send_command(command)

	def voice_set_sustain_release(self, voice: int, sustain: int, release:int):
		address = self.voice_map[voice] + SidRegisters.SID_VOICE_SUS_REL
		self.sid_state[address] = (sustain % 16) << 4;
		self.sid_state[address] += (release % 16)
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address])
		self.send_command(command)

	def voice_gate(self, voice: int, set: bool):
		address = self.voice_map[voice] + SidRegisters.SID_VOICE_CONTROL
		if set:
			self.sid_state[address] |= 0x01
		else:
			self.sid_state[address] &= ~(0x01)
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address])
		self.send_command(command)

	# Filter

	def filter_enable_filt(self, voice: int, enabled: bool):
		if(enabled):
			self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT] |= ( 1 << (voice & 0x03 ))
		else:
			self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT] &= ~(1 << (voice & 0x03 ))

		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_RES_FILT) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT])
		self.send_command(command)

	def filter_enable_lp(self, enabled: bool):
		if enabled:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] |= 0b00010000
		else:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] &= ~(0b00010000)

		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_MODE_VOL) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL])
		self.send_command(command)

	def filter_enable_bp(self, enabled: bool):
		if enabled:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] |= 0b00100000
		else:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] &= ~(0b00100000)
		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_MODE_VOL) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL])
		self.send_command(command)

	def filter_enable_hp(self, enabled: bool):
		if enabled:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] |= 0b01000000
		else:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] &= ~(0b01000000)
		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_MODE_VOL) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL])
		self.send_command(command)

	def filter_disable_voice_3(self, disable: bool):
		if disable:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] |= 0b10000000
		else:
			self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] &= ~(0b10000000)
		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_MODE_VOL) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL])
		self.send_command(command)

	def filter_set_volune(self, volume):
		self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] &= ~(0x0F)
		self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL] |= (volume % 16)
		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_MODE_VOL) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_MODE_VOL])
		self.send_command(command)

	def filter_set_fc(self, freq):
		address = SidRegisters.SID_FILTER_FC_LO
		self.sid_state[address] = ( freq & 0x0007 )
		self.sid_state[address+1] = (freq >> 3)
		command = "W" + '{:02x}'.format(address) + '{:02x}'.format(self.sid_state[address]) + '{:02x}'.format(self.sid_state[address+1])
		self.send_command(command)

	def filter_set_resonance(self, resonance):
		self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT] &= ~(0xF0)
		self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT] |= ( resonance << 4 )
		command = "W" + '{:02x}'.format(SidRegisters.SID_FILTER_FC_RES_FILT) + '{:02x}'.format(self.sid_state[SidRegisters.SID_FILTER_FC_RES_FILT])
		self.send_command(command)
