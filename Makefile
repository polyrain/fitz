.PHONY = clean all

CFLAGS = -Wall -pedantic -std=c99
DEBUG = -g
TARGETS = fitz

all: $(TARGETS)

debug: CFLAGS += $(DEBUG)
debug: clean $(TARGETS)

fitz: fitz.c
	gcc $(CFLAGS) fitz.c -o fitz

clean:
	rm -f $(TARGETS) *.o


