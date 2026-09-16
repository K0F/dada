CC      ?= cc
CFLAGS  ?= -O2 -std=c11 -Wall -Wextra

all: dada

dada: main.c
	$(CC) $(CFLAGS) -o $@ $<

test: dada
	@./dada seed > /tmp/dada.1 && ./dada seed > /tmp/dada.2 && diff -q /tmp/dada.1 /tmp/dada.2 \
	 && ./dada bytes 64 > /tmp/dada.1 && ./dada bytes 64 > /tmp/dada.2 && diff -q /tmp/dada.1 /tmp/dada.2 \
	 && ./dada pick the-oracle-is-closed lunch-is-oracular cheese-decides noise-is-truth > /tmp/dada.1 \
	 && ./dada pick the-oracle-is-closed lunch-is-oracular cheese-decides noise-is-truth > /tmp/dada.2 \
	 && diff -q /tmp/dada.1 /tmp/dada.2 \
	 && ./dada stream xor 32 > /tmp/dada.1 && ./dada stream xor 32 > /tmp/dada.2 && diff -q /tmp/dada.1 /tmp/dada.2 \
	 && ./dada stream bit-plane-3 16 > /tmp/dada.1 && ./dada stream bit-plane-3 16 > /tmp/dada.2 && diff -q /tmp/dada.1 /tmp/dada.2 \
	 && echo "DETERMINISM HOLDS — same stone, same world, forever."

clean:
	rm -f dada