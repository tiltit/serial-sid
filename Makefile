PRG = serial-sid

MCU_TARGET = atmega328p

F_CPU   = 8000000

DEFS =
LIBS =

CC = avr-gcc

OBJCOPY	= avr-objcopy
OBJDUMP	= avr-objdump

SOURCES = main.c cb.c helpers.c
OBJECTS = $(SOURCES:.c=.o)
DEPS    = cb.h helpers.h

all: $(PRG).hex

%.o: %.c $(DEPS)
	$(CC) -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU_TARGET) -c $<

$(PRG).elf: $(OBJECTS)
	$(CC) -mmcu=$(MCU_TARGET) $(OBJECTS) -o $(PRG).elf

$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -O ihex -R .eeprom $(PRG).elf $(PRG).hex

$(PRG).lst: $(PRG).elf
	$(OBJDUMP) -h -S $(PRG).elf > $(PRG).lst

$(PRG).map: $(PRG).elf $(OBJECTS)
	$(CC) -g -mmcu=$(MCU_TARGET) -Wl,-Map,$(PRG).map -o $(PRG).elf $(OBJECTS)


flash: $(PRG).hex
	# avrdude -F -V -c stk500v2  -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$(PRG).hex
	avrdude -F -V -c arduino  -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:$(PRG).hex

clean:
	rm -rf *.o *.elf *.lst *.map *.hex

