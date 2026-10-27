APP     := app
SRC     := main.c
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 $(shell pkg-config --cflags raylib)
LDFLAGS := $(shell pkg-config --libs raylib) -lm

.PHONY: all run clean

all: $(APP)

$(APP): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
