include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=$(MODULE:.bin=.elf)
SOURCES=$(wildcard [^_]*.c)

all: $(MODULE)

debug: all
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../$(MODULE_ELF)

$(MODULE): $(SOURCES)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)

clean:
	rm -rf *.o

.PHONY: all clean print
