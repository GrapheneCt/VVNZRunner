PREFIX=mep-elf-
CC=$(PREFIX)gcc
CFLAGS=-nostdlib -std=c99 -mtiny=0 -mabsdiff -maverage -mbitops -mclip -mdiv -mleadz -mminmax -mmult -mrepeat -msatur -mvl64 -mvliw -mcop64 -D__MEP_CONFIG_CP_DATA_BUS_WIDTH=64 -mivc2 -O3
LD=$(PREFIX)gcc
LDFLAGS=-nodefaultlibs -nostdlib -nostartfiles
OBJCOPY=$(PREFIX)objcopy
OBJCOPYFLAGS=

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
EXEC=mep_code

LIBS = -lpdclib -lgcc -lmempool

all: output/$(EXEC).elf

%.S: %.c
	$(CC) -S -o $@ $< $(CFLAGS)
	
%.o: %.S
	mv $< $<.w
	awk '{if ($$1 == "movu" && !($$3 ~ /^[0-9]/)) {$$1 = "or3"; $$4 = $$3; $$3 = $$2; printf("\tmovh %s 0x1234\n\t", $$2); printf("\thalt\n\t")}} 1' $<.w > $<
	rm $<.w
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)
	
output/$(EXEC).elf: $(EXEC).elf
	-rm -rf output
	mkdir output
	mv $(EXEC).elf output/$(EXEC).elf

clean:
	-rm src/*.o
	-rm -rf output