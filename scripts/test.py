#!/usr/bin/python3
from sid import Sid

import sys, getopt, select, os, time


def main(argv):

	# 7th octave
	#        do      do#     re      re#     mi      fa      fa#     sol     s#      la      la#     si
	notes = [0x892d, 0x9155, 0x99fa, 0xa321, 0xacd5, 0xb71c, 0xc1ff, 0xcd88, 0xd9c1, 0xe6b4, 0xf46c, 0x102f5]


	serial = ""
	try:
		opts, args = getopt.getopt(argv,"hs:",["serialport="])
	except getopt.GetoptError:
		print ('test.py -s <serialport>')
		sys.exit(2)

	for opt, arg in opts:
		if opt == '-h':
			print('test.py -s <serialport>')
			print('ex: test.py -s /dev/ttyUSB0')
			sys.exit()
		elif opt in ("-s", "--serialport"):
			serial = arg

	if serial == '':
		print('test.py -s <serialport>')
		sys.exit()
		


	sid = Sid("/dev/ttyUSB0")

	sid.filter_set_volune(15)

	# La
	sid.voice_waveform_triangle(0, True);
	sid.voice_set_attack_decay(0,9,7);
	sid.voice_set_sustain_release(0,5,9);
	sid.voice_set_freq(0, notes[5] >> 3)

	sid.voice_gate(0, True)
	time.sleep(0.6)
	sid.voice_gate(0, False)
	time.sleep(1)

	# Gamme
	sid.voice_set_attack_decay(0,9,5);
	sid.voice_set_sustain_release(0,2,6);

	for note in notes:
		sid.voice_set_freq(0, note >> 3)
		sid.voice_gate(0, True)
		time.sleep(0.4)
		sid.voice_gate(0, False)
		time.sleep(0.2)

	time.sleep(1)

	# Second voice with filter
	sid.filter_enable_filt(1, True)
	sid.filter_set_fc(1500)
	sid.filter_enable_lp(True)

	sid.voice_waveform_sawtooth(1, True);
	sid.voice_set_attack_decay(1,4,1);
	sid.voice_set_sustain_release(1,5,2);

	for note in notes:
		sid.voice_set_freq(0, note >> 3)
		sid.voice_set_freq(1, note >> 2)
		sid.voice_gate(0, True)
		sid.voice_gate(1, True)
		time.sleep(0.4)
		sid.voice_gate(0, False)
		sid.voice_gate(1, False)
		time.sleep(0.2)

	time.sleep(1)

	# Third Voice
	sid.voice_waveform_noise(2, True);
	sid.voice_set_attack_decay(2,4,1);
	sid.voice_set_sustain_release(2,5,2);
	sid.filter_enable_filt(2, True)

	for note in notes:
		sid.voice_set_freq(0, note >> 3)
		sid.voice_set_freq(1, note >> 2)
		sid.voice_set_freq(2, note >> 4)
		sid.voice_gate(0, True)
		sid.voice_gate(1, True)
		sid.voice_gate(2, True)
		time.sleep(0.4)
		sid.voice_gate(0, False)
		sid.voice_gate(1, False)
		sid.voice_gate(2, False)
		time.sleep(0.2)

	time.sleep(1)

	# Change voice 1 to square and sync
	sid.voice_sync(0, True)
	sid.voice_waveform_square(0, True);
	sid.voice_waveform_triangle(0, False);
	sid.filter_enable_filt(0, True)
	for note in notes:
		sid.voice_set_freq(0, note >> 3)
		sid.voice_set_freq(1, note >> 2)
		sid.voice_set_freq(2, note >> 4)
		sid.voice_gate(0, True)
		sid.voice_gate(1, True)
		sid.voice_gate(2, True)
		time.sleep(0.4)
		sid.voice_gate(0, False)
		sid.voice_gate(1, False)
		sid.voice_gate(2, False)
		time.sleep(0.2)

	sid.voice_waveform_square(0, False);
	sid.filter_enable_filt(0, False)

	time.sleep(1)



if __name__ == "__main__":
	main(sys.argv[1:])