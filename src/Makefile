
.PHONY: clean lst text eeprom

SOURCES        =
SOURCES       += bug.c
SOURCES       += isr.c
SOURCES       += message-queue.c
SOURCES       += edsm.c
SOURCES       += main.c

SOURCES       += sm/led.c
SOURCES       += usart.c
#SOURCES       += eeprom.c
SOURCES       += shell.c

OBJECTS        = $(SOURCES:.c=.o)
PROGRAM        = test
TARGET         = atmega328p
OPTIMIZE       = -Os

DEFS           = -DF_CPU=16000000UL
LIBS           =

### ---
### ---

CC = avr-gcc
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf lst text eeprom
#all: $(PROGRAM).elf lst text

$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -rf *.o $(PROGRAM).elf *.eps *.png *.pdf *.bak
	rm -rf *.lst .map $(EXTRA_CLEAN_FILES)

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PROGRAM).hex
bin:  $(PROGRAM).bin
srec: $(PROGRAM).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PROGRAM)_eeprom.hex
ebin:  $(PROGRAM)_eeprom.bin
esrec: $(PROGRAM)_eeprom.srec

%_eeprom.hex: %.elf
	@$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	@$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	@$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PROGRAM).eps
png: $(PROGRAM).png
pdf: $(PROGRAM).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@
