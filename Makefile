
CC ?= cc
BIN ?= clib-search
PREFIX ?= /usr/local
SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*.c)
CFLAGS = -std=c99 -Wall -Ideps -Isrc
LDFLAGS = -lcurl

$(BIN): $(SRC)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(BIN)

test: $(BIN)
	./$(BIN) ends runner cron

install: $(BIN)
	mkdir -p $(PREFIX)/bin
	cp -f $(BIN) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

clean:
	rm -f $(BIN)

.PHONY: install uninstall test clean
