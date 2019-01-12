CC = gcc
CFLAGS = -Ofast
FILES = $(shell find -name "*.c")

program: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o excerciseI -lpthread

.PHONY: clean

clean:
	rm -rf excerciseI
