
LIBS = -lcurl
INC = -Ideps
SRC = $(wildcard src/*.c)
SRC += $(wildcard deps/*.c)
BIN = clib-search

build: $(SRC)
	$(CC) -std=c99 $^ $(LIBS) $(INC) -o $(BIN)

test: build
	./$(BIN) ends runner cron
