CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
OBJFILES = tema1.o utils.o data_structures.o
TARGET = tema1

.PHONY: build clean run

build: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o ./$@ $(OBJFILES)

tema1.o: tema1.c data_structures.h utils.h
	$(CC) $(CFLAGS) -c tema1.c

data_structures.o: data_structures.c data_structures.h
	$(CC) $(CFLAGS) -c data_structures.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f ./$(TARGET) *.o

run: $(TARGET)
	./$(TARGET)

